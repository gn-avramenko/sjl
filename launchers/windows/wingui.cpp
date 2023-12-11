#include "wincommon.h"
#include "resource.h"
#include <vector>
using namespace std;

void show_last_error();

typedef JNIIMPORT jint(JNICALL *JNI_createJavaVM)(JavaVM **pvm, JNIEnv **env, void *args);
DWORD dwExitCode = 0;
jclass obj;

void(JNICALL jniExitHook)(jint code)
{
	debug(L"exited");
	dwExitCode = code;
	close_log_file_handle();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow)
{
	if (!prepare(lpCmdLine, hInstance))
	{
		show_last_error();
		return 1;
	}
	try
	{
		vector<string> vmOptionLines;
		string predefinedOptions = LoadStdString(hInstance, IDS_VM_OPTIONS);
		if (!predefinedOptions.empty())
		{
			string delimiter = "|";
			size_t pos = 0;
			std::string token;
			while ((pos = predefinedOptions.find(delimiter)) != std::string::npos)
			{
				token = predefinedOptions.substr(0, pos);
				vmOptionLines.push_back(token);
				predefinedOptions.erase(0, pos + delimiter.length());
			}
			vmOptionLines.push_back(predefinedOptions);
		}

		wstring binDir = get_jvm_path() + L"\\bin";
		wstring dllName = binDir + L"\\server\\jvm.dll";
		if (!is_file_exist(dllName))
		{
			dllName = binDir + L"\\client\\jvm.dll";
			if (!is_file_exist(dllName))
			{
				throw_exception(L"unable to locate jvm.dll");
			}
		}

		SetEnvironmentVariableW(L"JAVA_HOME", binDir.c_str());
		SetDllDirectoryW(nullptr);
		SetCurrentDirectoryW(get_base_path().c_str());
		HMODULE hJVM = LoadLibraryW(dllName.c_str());
		if (!hJVM)
		{
			throw_exception(L"unable to load jvm from " + dllName);
		}
		JNI_createJavaVM pCreateJavaVM = NULL;
		if (hJVM)
		{
			pCreateJavaVM = (JNI_createJavaVM)GetProcAddress(hJVM, "JNI_CreateJavaVM");
		}
		if (!pCreateJavaVM)
		{
			throw_exception(L"unable to find address of JNI_CreateJavaVM");
		}
		int vmOptionsCount = 2+vmOptionLines.size();
		JavaVMOption *vmOptions = (JavaVMOption *)calloc(vmOptionsCount, sizeof(JavaVMOption));
		vmOptions[0].optionString = (char *)"exit";
		vmOptions[0].extraInfo = (void *)jniExitHook;
		string cp = LoadStdString(hInstance, IDS_CLASS_PATH);
		if (cp.empty())
		{
			throw_exception(L"classpath is not defined");
		}
		string cpOption = "-Djava.class.path=" + cp;
		vmOptions[1].optionString = (char *)cpOption.c_str();
		for (int i = 0; i < vmOptionLines.size(); i++)
		{
			string opt = vmOptionLines[i];
			int pos = opt.find("??");
			if( pos != string::npos){
				opt = opt.substr(0, pos) + opt.substr(pos+2);
			}
			vmOptions[i + 2].optionString = _strdup(opt.c_str());
			vmOptions[i + 2].extraInfo = NULL;		
		}
		JavaVMInitArgs vmArgs;
		vmArgs.version = JNI_VERSION_1_2;
		vmArgs.nOptions = vmOptionsCount;
		vmArgs.options = vmOptions;
		vmArgs.ignoreUnrecognized = JNI_TRUE;

		JNIEnv *jenv = NULL;
		JavaVM *jvm = NULL;
		int result = pCreateJavaVM(&jvm, &jenv, &vmArgs);
		for (int i = 1; i < vmOptionsCount; i++)
		{
			free(vmOptions[i].optionString);
		}
		free(vmOptions);
		vmOptions = NULL;
		if (result != JNI_OK)
		{
			throw_exception(L"unable to create jvm");
		}
		jthrowable jtExcptn;
		const std::string mainClassName = LoadStdString(hInstance, IDS_MAIN_CLASS);
		jclass mainClass = jenv->FindClass(mainClassName.c_str());
		if (!mainClass)
		{
			throw_exception(L"unable to find main class ");
		}

		jmethodID mainMethod = jenv->GetStaticMethodID(mainClass, "main", "([Ljava/lang/String;)V");
		if (!mainMethod)
		{
			throw_exception(L"unable to find main method ");
		}
		jclass stringClass = jenv->FindClass("java/lang/String");
		jobjectArray args = jenv->NewObjectArray(0, stringClass, NULL);

		jenv->CallStaticVoidMethod(mainClass, mainMethod, args);
		jthrowable exc = jenv->ExceptionOccurred();
		if (exc)
		{
			throw_exception(L"Error invoking main method");
		}
		jvm->DestroyJavaVM();

		// close_log_file_handle();
		return 0;
	}
	catch (...)
	{
		debug(L"error occurred");
		close_log_file_handle();
		show_last_error();
		return 1;
	}
}

void show_last_error()
{
	wstring msg = get_error_message();
	MessageBoxW(NULL, msg.empty() ? L"Unknown error" : msg.c_str(),
				L"Error occurred", MB_OK);
}