/*
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string>
#include <windows.h>
#include <vector>
#include "resource.h"
#include "jni.h"
#include "jni_md.h"

using namespace std;

// global variables
DWORD dwExitCode = 0;
bool debug_flag = false;
HINSTANCE hInst;
wstring base_path;
wstring executable_path;
wstring program_params;
wstring jvm_path;
wstring exception_message;
wstring sjl_path;
HANDLE log_file;

// functions declaration
void show_last_error();
void debug(wstring message);
void debug(string message);
void close_log_file_handle();
void trim_line(char *line);
void add_option(string token, vector<string> &vmOptionLines);
string to_string(const std::wstring &str);
void init_paths();
wstring get_wstring_param(int id);
void init_debug_file();
bool is_file_exist(wstring path);
void throw_exception(wstring message);
HANDLE init_utf8_file(wstring file_name);
void init_sjl_dir();
void create_mutex();
string get_string_param(int id);
wstring to_wstring(string s);

// jni declarations
typedef JNIIMPORT jint(JNICALL *JNI_createJavaVM)(JavaVM **pvm, JNIEnv **env, void *args);

void(JNICALL jniExitHook)(jint code)
{
	debug("exited");
	dwExitCode = code;
	string restartCode = get_string_param(IDS_RESTART_EXIT_CODE);
	close_log_file_handle();
	if(code == stoi(restartCode)){
		ShellExecuteW(NULL, L"open", executable_path.c_str(), program_params.c_str(), NULL, SW_SHOWDEFAULT);
	}
}

// main function
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	string cmdLineStr = lpCmdLine;
	program_params =  lpCmdLine == NULL? L"": to_wstring(cmdLineStr);
	debug_flag = cmdLineStr.find("-sjl-debug") != string::npos || true;
	try
	{
		init_paths();
		init_debug_file();
		debug("command line is :" + ((string)lpCmdLine));
		create_mutex();
		_wchdir(base_path.c_str());
		debug(L"Working dir:\t" + base_path);

		vector<string> vmOptionLines;
		string path = get_string_param(IDS_VM_OPTIONS_FILE);
		if (!path.empty())
		{

			FILE *f;
			if (!fopen_s(&f, path.c_str(), "rt"))
			{

				char buffer[4096];
				while (fgets(buffer, sizeof(buffer), f))
				{
					trim_line(buffer);
					if (strlen(buffer) > 0 && buffer[0] != '#' && strcmp(buffer, "-server") != 0)
					{
						std::string line(buffer);
						vmOptionLines.push_back(line);
					}
				}
				fclose(f);
			}
		}

		string predefinedOptions = get_string_param(IDS_VM_OPTIONS);
		if (!predefinedOptions.empty())
		{
			string delimiter = "|";
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

		wstring binDir = jvm_path + L"\\bin";
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
		SetCurrentDirectoryW(base_path.c_str());
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
		int vmOptionsCount = 2 + vmOptionLines.size();
		JavaVMOption *vmOptions = (JavaVMOption *)calloc(vmOptionsCount, sizeof(JavaVMOption));
		vmOptions[0].optionString = (char *)"exit";
		vmOptions[0].extraInfo = (void *)jniExitHook;
		string cp = get_string_param(IDS_CLASS_PATH);
		if (cp.empty())
		{
			throw_exception(L"classpath is not defined");
		}
		string cpOption = "-Djava.class.path=" + cp;
		vmOptions[1].optionString = (char *)cpOption.c_str();
		for (int i = 0; i < vmOptionLines.size(); i++)
		{
			string opt = vmOptionLines[i];

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
		const std::string mainClassName = get_string_param(IDS_MAIN_CLASS);
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
	MessageBoxW(NULL, exception_message.empty() ? L"Unknown error" : exception_message.c_str(),
				L"Error occurred", MB_OK);
}

void trim_line(char *line)
{
	char *p = line + strlen(line) - 1;
	if (p >= line && *p == '\n')
	{
		*p-- = '\0';
	}
	while (p >= line && (*p == ' ' || *p == '\t'))
	{
		*p-- = '\0';
	}
}

void add_option(string token, vector<string> &vmOptionLines)
{
	int markerPos = token.find("??");
	if (markerPos != string::npos)
	{
		bool found = false;
		string start = token.substr(0, markerPos);
		for (int n = 0; n < vmOptionLines.size(); n++)
		{
			if (vmOptionLines[n].find(start) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			vmOptionLines.push_back(token.substr(0, markerPos) + token.substr(markerPos + 2));
		}
	}
	else
	{
		vmOptionLines.push_back(token);
	}
}

void debug(wstring message)
{
	if (debug_flag)
	{
		wstring result = message + L"\n";
		DWORD dwBytesWritten = 0;
		string strBuf = to_string(result);
		WriteFile(log_file, strBuf.c_str(), strBuf.size(), &dwBytesWritten,
				  NULL);
	}
}

void debug(string message)
{
	if (debug_flag)
	{
		string result = message + "\n";
		DWORD dwBytesWritten = 0;
		WriteFile(log_file, result.c_str(), result.size(), &dwBytesWritten,
				  NULL);
	}
}

string to_string(const wstring &str)
{
	const int cbANSI = WideCharToMultiByte(CP_ACP, 0, str.c_str(), (int)str.size(), NULL, 0, NULL, NULL);
	if (cbANSI <= 0)
		return string();

	char *ansiBuf = new char[cbANSI];
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), (int)str.size(), ansiBuf, cbANSI, NULL, NULL);
	string result(ansiBuf, cbANSI);
	delete[] ansiBuf;
	return result;
}

void init_paths()
{
	wchar_t path[MAX_PATH]{};
	GetModuleFileNameW(NULL, path, MAX_PATH);
	wstring pathStr = path;
	executable_path = path;
	size_t position = pathStr.find_last_of(L"\\");
	base_path = pathStr.substr(0, position);
	jvm_path = get_wstring_param(IDS_JVM_PATH);
}

wstring get_wstring_param(int id)
{
	wchar_t *buf = NULL;
	int len = LoadStringW(hInst, id, reinterpret_cast<LPWSTR>(&buf), 0);
	return len ? std::wstring(buf, len) : L"";
}

void init_debug_file()
{
	if (debug_flag)
	{
		init_sjl_dir();
		wstring log_file_name = sjl_path + L"\\sjl.log";
		if (is_file_exist(log_file_name) && !DeleteFileW(log_file_name.c_str()))
		{
			throw_exception(L"unable to delete file " + log_file_name);
		}
		log_file = init_utf8_file(log_file_name);
	}
}

bool is_file_exist(wstring path)
{
	LPCWSTR szPath = path.c_str();
	DWORD dwAttrib = GetFileAttributesW(szPath);

	return dwAttrib != INVALID_FILE_ATTRIBUTES;
}

HANDLE init_utf8_file(wstring file_name)
{
	if (is_file_exist(file_name) && !DeleteFileW(file_name.c_str()))
	{
		throw_exception(L"unable to delete file " + file_name);
	}

	HANDLE handle = CreateFileW(file_name.c_str(), GENERIC_WRITE, 0, NULL,
								CREATE_ALWAYS, 0, NULL);
	unsigned char smarker[3];
	DWORD bytesWritten;

	smarker[0] = 0xEF;
	smarker[1] = 0xBB;
	smarker[2] = 0xBF;
	WriteFile(handle, smarker, 3, &bytesWritten, NULL);
	return handle;
}

void init_sjl_dir()
{
	sjl_path = base_path + L"\\.sjl";
	if (!is_file_exist(sjl_path) && !CreateDirectoryW(sjl_path.c_str(), NULL))
	{
		throw_exception(L"unable to create directory " + sjl_path);
	}
}

void throw_exception(wstring message)
{
	exception_message = message;
	throw exception();
}

void create_mutex()
{
	wstring mutexName = get_wstring_param(IDS_MUTEX_NAME);

	if (!mutexName.empty())
	{
		SECURITY_ATTRIBUTES security;
		security.nLength = sizeof(SECURITY_ATTRIBUTES);
		security.bInheritHandle = TRUE;
		security.lpSecurityDescriptor = NULL;
		CreateMutexW(&security, FALSE, mutexName.c_str());

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			debug("Instance already exists.");
			throw_exception(L"application instance is already launched");
		}
		debug(L"Create mutex:\t" + mutexName);
	}
}

string get_string_param(int id)
{
  wchar_t *buf = NULL;
  int len = LoadStringW(hInst, id, reinterpret_cast<LPWSTR>(&buf), 0);
  return len ? to_string(wstring(buf, len)) : "";
}

void close_log_file_handle() {
	if(log_file){
		CloseHandle(log_file);
	}
}

wstring to_wstring(string s)
{
    std::wstring ws(s.size(), L' '); // Overestimate number of code points.
    ws.resize(std::mbstowcs(&ws[0], s.c_str(), s.size()));
	return ws;
}