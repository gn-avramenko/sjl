#include "JVM.h"
#include "jni.h"
#include "jni_md.h"
#include <vector>
#include <string>
#include "Utils.h"
#include "Windows.h"

typedef JNIIMPORT jint(JNICALL* JNI_createJavaVM)(JavaVM** pvm, JNIEnv** env, void* args);

Debug* hDebug;
Resources* hResources;
SingleInstanceChecker* hSic;
std::string programParams;
std::wstring executablePath;

void JVM::add_option(std::string token, std::vector<std::string>& vmOptionLines)
{
	debug->Log("analyzing predefined option %s", token.c_str());
	int markerPos = token.find("??");
	if (markerPos != std::string::npos)
	{
		debug->Log("marker ?? exists");
		bool found = false;
		std::string start = token.substr(0, markerPos);
		for (int n = 0; n < vmOptionLines.size(); n++)
		{
			if (vmOptionLines[n].find(start) == 0)
			{
				debug->Log("found existing option %s", vmOptionLines[n].c_str());
				found = true;
				break;
			}
		}
		if (!found)
		{
			std::string option = token.substr(0, markerPos) + token.substr(markerPos + 2);
			debug->Log("adding option %s", option.c_str());
			vmOptionLines.push_back(option);
		}
	}
	else
	{
		debug->Log("option added %s", token.c_str());
		vmOptionLines.push_back(token);
	}
}

std::wstring JVM::findJavaHome()
{
	return resources->GetEmbeddedJavaHomePath();	
}

void(JNICALL jniExitHook)(jint code)
{
	hDebug->Log("inside jni exit hook");
	int restartCode = hResources->GetRestartExitCode();
	hSic->MutexRelease();
	if (code == restartCode)
	{
		std::wstring newParams = to_wstring_(programParams);
		if (newParams.find(L"-sjl-restart") == std::string::npos)
		{
			newParams = newParams + L" -sjl-restart";
		}

		ShellExecuteW(NULL, L"open", executablePath.c_str(), newParams.c_str(), NULL, SW_RESTORE);
	}
}

JVM::JVM(ExceptionWrapper* ew, Locations* loc, Debug* deb, Resources* res, SingleInstanceChecker* sic, SplashScreen* splash, std::string progParams)
{
	exceptionWrapper = ew;
	locations = loc;
	debug = deb;
	hDebug = deb;
	resources = res;
	hResources = res;
	hSic = sic;		 
	splashScreen = splash;
	programParams = progParams;
	executablePath = locations->GetExecutablePath();
}

void JVM::LaunchJVM()
{
	std::vector<std::string> vmOptionLines;
	std::wstring optionsFile = locations->GetOptionsFile();
	if (!optionsFile.empty())
	{
		debug->Log(L"reading options file %s", optionsFile.c_str());
		if (locations->FileExists(optionsFile))
		{
			debug->Log("file exists");
			FILE* f;
			int res = _wfopen_s(&f, optionsFile.c_str(), L"rt");
			if (!res && f)
			{
				char buffer[4096];
				while (fgets(buffer, sizeof(buffer), f))
				{
					trim_line(buffer);
					if (strlen(buffer) > 0 && buffer[0] != '#' && strcmp(buffer, "-server") != 0)
					{
						std::string line(buffer);
						debug->Log("added line %s", line);
						vmOptionLines.push_back(line);
					}
				}
				fclose(f);
			}
			else
			{
				exceptionWrapper->ThrowException(format_message(L"unable to open file %s, code = %s", optionsFile.c_str(), res), format_message(resources->GetUnableToOpenFileMessage(), optionsFile.c_str()));
			}
		}
	}

	std::string predefinedOptions = resources->GetVMOptions();
	if (!predefinedOptions.empty())
	{
		debug->Log("predefined options exist");
		std::string delimiter = "|";
		size_t pos = 0;
		std::string token;
		while ((pos = predefinedOptions.find(delimiter)) != std::string::npos)
		{
			token = predefinedOptions.substr(0, pos);
			add_option(token, vmOptionLines);

			predefinedOptions.erase(0, pos + delimiter.length());
		}
		add_option(predefinedOptions, vmOptionLines);
	}
	std::wstring javaHome = findJavaHome();
	std::wstring binDir = javaHome + L"\\bin";
	std::wstring dllName = binDir + L"\\server\\jvm.dll";
	if (!locations->FileExists(dllName))
	{
		debug->Log(L"file %s does not exist", dllName.c_str());
		dllName = binDir + L"\\client\\jvm.dll";
		if (!locations->FileExists(dllName))
		{
			debug->Log(L"file %s does not exist",  dllName.c_str());
			exceptionWrapper->ThrowException(format_message(L"unable to locate jvm.dll in %s", binDir.c_str()), format_message(resources->GetUnableToLocateJvmDllMessage(), binDir.c_str()));
		}
	}
	std::string cp = resources->GetClassPath();
	if (cp.empty())
	{
		exceptionWrapper->ThrowException(L"classpath is not defined", resources->GetClassPathIsNotDefinedMessage());
	}
	std::string cpOption = "-Djava.class.path=" + cp;
	debug->Log("class path is set to %s", cp.c_str());
	SetEnvironmentVariableW(L"JAVA_HOME", binDir.c_str());
	SetDllDirectoryW(nullptr);
	SetCurrentDirectoryW(locations->GetBasePath().c_str());
	debug->Log(L"current directory is set to %s", locations->GetBasePath().c_str());
	HMODULE hJVM = LoadLibraryW(dllName.c_str());
	if (!hJVM)
	{
		exceptionWrapper->ThrowException(format_message(L"unable to load jvm from: %s lastError: %d", dllName.c_str(), GetLastError()),
			format_message(resources->GetUnableToLoadJvmDllMessage(), dllName.c_str()));
	}
	JNI_createJavaVM pCreateJavaVM = NULL;
	if (hJVM)
	{
		pCreateJavaVM = (JNI_createJavaVM)GetProcAddress(hJVM, "JNI_CreateJavaVM");
	}
	if (!pCreateJavaVM)
	{
		exceptionWrapper->ThrowException(L"unable to find address of JNI_CreateJavaVM", resources->GetUnableToFindAdressOfJNI_CreateJavaVMMessage());
	}
	int vmOptionsCount = 2 + vmOptionLines.size();
	JavaVMOption* vmOptions = (JavaVMOption*)calloc(vmOptionsCount, sizeof(JavaVMOption));
	vmOptions[0].optionString = (char*)"exit";
	vmOptions[0].extraInfo = (void*)jniExitHook;
	vmOptions[1].optionString = (char*)cpOption.c_str();
	for (int i = 0; i < vmOptionLines.size(); i++)
	{
		std::string opt = vmOptionLines[i];

		vmOptions[i + 2].optionString = _strdup(opt.c_str());
		vmOptions[i + 2].extraInfo = NULL;
	}

	JavaVMInitArgs vmArgs;
	vmArgs.version = JNI_VERSION_1_2;
	vmArgs.nOptions = vmOptionsCount;
	vmArgs.options = vmOptions;
	vmArgs.ignoreUnrecognized = JNI_TRUE;

	JNIEnv* jenv = NULL;
	JavaVM* jvm = NULL;
	int result = pCreateJavaVM(&jvm, &jenv, &vmArgs);
	for (int i = 1; i < vmOptionsCount; i++)
	{
		free(vmOptions[i].optionString);
	}

	free(vmOptions);
	vmOptions = NULL;
	if (result != JNI_OK)
	{
		exceptionWrapper->ThrowException(format_message(L"unable to create jvm, code = %d", result), resources->GetUnableToCreateJVMMessage());		
	}
	debug->Log("jvm was created");
	jthrowable jtExcptn;
	//ManualSynchronization on 18.01.24 23:26
	//const std::string mainClassName = get_string_param(IDS_MAIN_CLASS);
	const std::string mainClassName = resources->GetMainClass();
	debug->Log("main class is %s", mainClassName.c_str());
	jclass mainClass = jenv->FindClass(mainClassName.c_str());
	if (!mainClass)
	{
		exceptionWrapper->ThrowException(format_message(L"unable to find main class %s", to_wstring_(mainClassName).c_str()),
			format_message(resources->GetUnableToFindMainClassMessage(), to_wstring_(mainClassName).c_str()));
	}

	jmethodID mainMethod = jenv->GetStaticMethodID(mainClass, "main", "([Ljava/lang/String;)V");
	if (!mainMethod)
	{
		exceptionWrapper->ThrowException(L"unable to find main method",resources->GetUnableToFindMainMethodMessage());
	}
	jclass stringClass = jenv->FindClass("java/lang/String");
	jobjectArray args = jenv->NewObjectArray(0, stringClass, NULL);
	jenv->CallStaticVoidMethod(mainClass, mainMethod, args);
	jthrowable exc = jenv->ExceptionOccurred();
	if (exc)
	{
		exceptionWrapper->ThrowException(L"error invoking main method", resources->GetErrorInvokingMainMethodMessage());
	}
	splashScreen->HideSplash();
	//ManualSynchronization 12.01.24 00:05
	//hide_current_splash();
	//ShowWindow(main_app_window_handle, SW_HIDE);
	//ManualSynchronization ends
	//activate_window();
	jvm->DestroyJavaVM();
	debug->Log("jvm was destroyed");
	////ManualSynchronization ends
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
	debug->Log("exit from LaunchJVM");
}
