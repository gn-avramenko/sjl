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
//ManualSynchronization on 19.01.24 00:40
bool g_debug_enabled = true; // Set to false to disable debug mode

//ManualSynchronization on 24.12.23 06:36
//#include <string>
//#include <windows.h>
#include <windows.h>
#include <string>
//ManualSynchronization ends

#include <vector>
#include "resource.h"
#include "jni.h"
#include "jni_md.h"
#include <time.h>

using namespace std;
// some constants
#define UPDATE_STATE_READING_TYPE 1
#define UPDATE_STATE_READING_MOVE_SOURCE 2
#define UPDATE_STATE_READING_MOVE_TARGET 3
#define UPDATE_STATE_READING_DELETE_FILE 4
#define UPDATE_STATE_READING_SLEEP_TIMEOUT 5
#define UPDATE_STATE_READING_SPLASH_FILE 6
#define SPLASH_WINDOW_CLASS_NAME L"SplashWindow"

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
HANDLE mutex = NULL;
HANDLE log_file;
HANDLE splash_screen_thread;
HBITMAP h_splash_bitmap, h_splash_old_bitmap;
HDC h_dc, h_dc_mem;
BITMAP splash_bm;
PAINTSTRUCT splash_ps;
RECT splash_rect;
HWND splash_window_handle;
DWORD process_id;

//ManualSynchronization on 10.01.24 14:15
bool launch_singleton_flag = false;
//ManualSynchronization on 10.01.24 14:39
HWND main_app_window_handle;
//ManualSynchronization on 11.01.24 18:02
#define MAX_LOADSTRING 100
//ManualSynchronization on 11.01.24 18:02
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
//ManualSynchronization ends
//ManualSynchronization on 11.01.24 23:35
bool g_bShow_splash_flag = false;
// functions declaration
void show_last_error();
//ManualSynchronization on 24.12.23 20:24
void debug_args(wchar_t* format, ...);
//ManualSynchronization on 24.12.23 18:40
void debug_args(char* format, ...);
//ManualSynchronization on 24.12.23 07:48
void debug_(const char* format, ...);
//ManualSynchronization ends
void debug(wstring message);
void debug(string message);
void close_log_file_handle();
void trim_line(char *line);
void trim_line(wchar_t *line);
void add_option(string token, vector<string> &vmOptionLines);
//ManualSynchronization on 24.12.23 06:21
string to_string_(const std::wstring &str);
void init_paths();
wstring get_wstring_param(int id);
void init_debug_file();
bool is_file_exist(wstring path);
bool is_directory(wstring path);
void throw_exception(wstring message);
HANDLE init_utf8_file(wstring file_name);
void init_sjl_dir();
void create_mutex();
string get_string_param(int id);
//ManualSynchronization on 23.12.23 14:46
wstring to_wstring_(string s);
//ManualSynchronization on 18.01.24 23:01
int LoadStringFromResourceW(HINSTANCE hInstance, LPWSTR ResourceName, LPWSTR Buffer, int cchBufferMax);
int LoadStringFromResource(HINSTANCE hInstance, char* ResourceName, char* Buffer, int cchBufferMax);
wstring get_resourcestring_paramW(wchar_t* resourcename);
string get_resourcestring_param(char* resourcename);

void copy_directory(wstring source, wstring target);
void remove_directory(wstring dir);
void show_splash(wstring imagePath);
void hide_current_splash();
LRESULT CALLBACK splash_window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void activate_window();
// jni declarations
typedef JNIIMPORT jint(JNICALL *JNI_createJavaVM)(JavaVM **pvm, JNIEnv **env, void *args);

void(JNICALL jniExitHook)(jint code)
{
	debug("exited");
	dwExitCode = code;
	//ManualSynchronization on 18.01.24 23:24
	//string restartCode = get_string_param(IDS_RESTART_EXIT_CODE);
	string restartCode = get_resourcestring_param("RESTART_EXIT_CODE");
	if (mutex)
	{
		ReleaseMutex(mutex);
	}
	close_log_file_handle();
	if (code == stoi(restartCode))
	{
		wstring newParams = program_params;
		if (newParams.find(L"-sjl-restart") == string::npos)
		{
			newParams = newParams + L" -sjl-restart";
		}

		ShellExecuteW(NULL, L"open", executable_path.c_str(), newParams.c_str(), NULL, SW_RESTORE);		
	}
}

//ManualSynchronization on 10.01.24 14:19
HANDLE CheckOneInstanceLaunched();
//ManualSynchronization ends
//ManualSynchronization on 11.01.24 17:44
void InitResourceData();
//ManualSynchronization ends
//ManualSynchronization on 11.01.24 18:08
ATOM					MyRegisterClass(HINSTANCE hInstance);
BOOL					InitInstance(HINSTANCE hinstance, int cmd);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);
//ManualSynchronization ends


//Synchronization on 11.01.24 19:42
struct EnumWindowsData {
	const wchar_t* targetTitle;
	HWND hwnd;
};
int EnumTimes = 0;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	EnumWindowsData* data = reinterpret_cast<EnumWindowsData*>(lParam);

	// Check if the window has the specified title
	wchar_t title[256];
	if (GetWindowTextW(hwnd, title, sizeof(title)) > 0 && wcscmp(title, data->targetTitle) == 0)
  {
		data->hwnd = hwnd;
		//EnumTimes++;
		//if(EnumTimes>1)
		if(hwnd != main_app_window_handle)
			return FALSE;  // Stop enumerating
	}
	

	return TRUE;  // Continue enumerating
}
BOOL CALLBACK EnumWindowsProc_JVM(HWND hwnd, LPARAM lParam) {
	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);
	debug_args("EnumWindowsProc_JVM GetWindowThreadProcessId %d ", processId);
	DWORD currentProcessID;
	currentProcessID = GetCurrentProcessId();
	if (currentProcessID == processId)
	{
		debug_args("EnumWindowsProc_JVM current Window %02X ", hwnd);
		wchar_t title[256];
		//GetWindowTextW(hwnd, title, sizeof(title));
		//MessageBoxW(0, L"Foreground Window Handle: ", title, 0);
		ShowWindow(hwnd, SW_HIDE);
		UpdateWindow(hwnd);
	}
	//if (IsWindowVisible(hwnd)) {
	//	// Check if the window is the foreground window
	//	//if (hwnd == GetForegroundWindow()) {
	//		wchar_t title[256];
	//		GetWindowTextW(hwnd, title, sizeof(title));
	//		MessageBoxW(0, L"Foreground Window Handle: ", title, 0);
	//		
	//		//hide_current_splash();
	//	//}
	//	//else {
	//	
	//	//}
	//}

	return TRUE;  // Continue enumerating
}
//Synchronization ends
//ManualSynchronization on 19.01.24 12:35
DWORD WINAPI EnumerateWindowsThread(LPVOID lpParam) {

	while (true)
	{
		Sleep(50);
		HWND targetWindow = FindWindowEx(nullptr, nullptr, nullptr, L"Java Swing Example-2");
		if (targetWindow)
		{
			//MessageBoxA(0, "found", 0, 0);
			hide_current_splash();
		}
	}

	//HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
	//EnumDesktopWindows(NULL, EnumWindowsProc_JVM, 0);
	////EnumWindows(EnumWindowsProc_JVM, 0);
	//if (hTaskbar != NULL) {
	//	// Enumerate child windows of the taskbar
	//	//EnumChildWindows(GetDesktopWindow(), EnumWindowsProc_JVM, 0);
	//
	//}
	//else {
	//	
	//}

	return 0;
}


//ManualSynchronization ends

//ManualSynchronization on 19.01.24 12:48
HHOOK g_hook;
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam) {
	//if (nCode == HCBT_CREATEWND)
	//{
		HWND hNewWindow = (HWND)wParam;
		debug_args("New Window Created: %02X ", hNewWindow);
		wchar_t title[256];
		GetWindowTextW(hNewWindow, title, sizeof(title));
		debug_args("New Window Title: %S ", title);
		GetClassName(hNewWindow, title, sizeof(title));
		debug_args("New Window Class: %S ", title);
		PostMessage(hNewWindow, WM_CLOSE, 0, 0);
		WaitForSingleObject(hNewWindow, INFINITE);
		
	//}

	return CallNextHookEx(g_hook, nCode, wParam, lParam);
}
//ManualSynchronization ends

//ManualSynchronization on 19.01.24 13:09

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
	//if (event == EVENT_OBJECT_CREATE) {
		// Get the length of the window text
	if (hwnd)
	{
		debug_args(L"WinEventProc: %02X ", event);
		//MessageBox(0, L"WinEventProc", 0, 0);
		int textLength = GetWindowTextLength(hwnd);
		if (textLength > 0) {
			// Allocate buffer for the window text
			wchar_t* windowText = new wchar_t[textLength + 1];

			// Get the window text
			GetWindowText(hwnd, windowText, textLength + 1);

			// Get the length of the window class name
			int classNameLength = 256; // Adjust as needed
			wchar_t* className = new wchar_t[classNameLength];

			// Get the window class name
			GetClassName(hwnd, className, classNameLength);

			// Display the window text and class name
			debug_args(L"New Window Text: %s ", windowText);
			debug_args(L"New Window Class: %s ", className);
			// Clean up allocated memory
			delete[] windowText;
			delete[] className;
		}
	}
	//}
}
//ManualSynchronization ends
// main function
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR pCmdLine, int nCmdShow)
{
	//ManualSynchronization starts on 10.01.24 16:36
	


	//ManualSynchronization ends
	//Sleep(15000);
	process_id = GetCurrentProcessId();
	hInst = hInstance;
	string cmdLineStr = pCmdLine;
	//ManualSynchronization on 24.12.23 07:37
	program_params = to_wstring_(cmdLineStr);
	//ManualSynchronization ends
	//ManualSycnhronization on 10.01.24 11:43
	//auto pTestCmdLine = pCmdLine;
	//ManualSynchronization on 10.01.24 11:45
	//debug_flag = cmdLineStr.find("-sjl-debug") != string::npos || true;
	//auto bTestFindDebug = (cmdLineStr.find("-sjl-debug") != string::npos);
	//debug_flag = bTestFindDebug ? true : false;
	//ManualSynchronization ends
	//ManualSynchronization on 10.01.24 12:29
	debug_flag = (cmdLineStr.find("-sjl-debug") != string::npos);
	//ManualSynchronization ends
	//ManualSynchronization on 10.01.24 14:16
	//launch_singleton_flag = (cmdLineStr.find("-sjl-launch-singleton") != string::npos);
	
	//ManualSynchronization on 11.01.24 18:06
	InitResourceData();
	//ManualSynchronization ends;
	//ManualSynchronization on 11.01.24 18:25
	//MyRegisterClass(hInstance);
	//InitInstance(hInstance, nCmdShow);
	//ManualSynchronization ends;
	try
	{
		if (cmdLineStr.find("-sjl-restart") != string::npos)
		{
			Sleep(200);
		}
		init_paths();
		init_debug_file();
		//ManualSynchronization on 24.12.23 18:39
		debug_args("new outputDebug with args firstArg:%s SecondArg2:%d ThirdArg3:%s", cmdLineStr.c_str(), 2, "third arg");
		//ManualSynchronization on 24.12.23 07:55
		//ManualSynchronization on 24.12.23 20:30
		debug_args(L"Unicode new outputDebug with args firstArg:%S SecondArg2:%d ThirdArg3:%s", cmdLineStr.c_str(), 2, L"third arg");
		//debug_("new outputDebug arg1:%s arg2:%d arg3:%s",cmdLineStr.c_str(),2,"third arg");
		debug("command line is :" + cmdLineStr);
		debug("nCmdShow is :" + to_string(nCmdShow));
		//ManualSynchronization starts on 10.01.24 14:17
		//create_mutex();
		//Init Main Window Handle
		//main_app_window_handle = FindWindowW(0);
		//Task#3
		if (launch_singleton_flag)
		{
			if (!CheckOneInstanceLaunched())
			{
				//ManualSynchronization on 10.01.24 17:44

				//HWND hwnd = FindWindowW(nullptr, szTitle);
				//if (hwnd)
				//{
				//	ShowWindow(hwnd, SW_RESTORE);
				//	SetForegroundWindow(hwnd);
				//	
				//}
				//else
				//	MessageBoxW(0, L"One instance allready running", 0, 0);
				EnumWindowsData data;
				data.targetTitle = szTitle;
				data.hwnd = nullptr;

				//ManualSynchronization ends
				//ManualSynchronization on 11.01.24 19:30
				DWORD pid = 0;
/*				GetWindowThreadProcessId(hwnd, &pid);
				DWORD currentProcessID = GetCurrentProcessId();
				if (pid != currentProcessID)
					MessageBoxW(0, L"not current process", 0, 0);
				else 
					MessageBoxW(0, L"current process", 0, 0);
			*/	
				EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));

				if (data.hwnd != nullptr) {
					// Bring the window to the foreground
					
					SetForegroundWindow(data.hwnd);
					
					//ShowWindow(data.hwnd, SW_RESTORE);
					//SendMessage(data.hwnd, (WM_USER+1), 0, 0);
					//if (data.hwnd != NULL) {
					//	HWND childWnd = FindWindowEx(data.hwnd, NULL, NULL, NULL); // Find the first child window
					//	if (childWnd != NULL) {
					//		SetForegroundWindow(childWnd);
					//		MessageBoxW(0, L"ChildWindow Is null", 0, 0);
					//	}
					//}
					/*GetWindowThreadProcessId(data.hwnd, &pid);
					DWORD currentProcessID = GetCurrentProcessId();
					if (pid != currentProcessID)
						MessageBoxW(0, L"not current process", 0, 0);
					else
						MessageBoxW(0, L"current process", 0, 0);*/

				}
				else {
					MessageBoxW(0, L"null hwnd", 0, 0);
				}
				//ManualSynchronization ends
				return 0;
			}
		}
		//ManualSynchronization ends
		_wchdir(base_path.c_str());
		debug(L"working dir:\t" + base_path);
		if (true)
		{
			//ManualSynchronization on 10.01.24 13:36
			//show_splash(L"c:\\IdeaProjects\\sjl\\launchers\\windows\\build\\sample.bmp");
			//ManualSynchronization ends
			//ManualSynchronization on 11.01.24 23:37
			if (g_bShow_splash_flag)
			{
				wstring splash_path = base_path + L"\\" + L"sample.bmp";
				show_splash(splash_path.c_str());
			}
			//ManualSynchronization ends
			//ManualSynchronization on 11.01.24 23:31
			//Sleep(2000);
			//hide_current_splash();		
			//ManualSynchronization ends
			//ManualSynchronization on 10.01.24 14:10
			//PostMessage(splash_window_handle, WM_SHOWWINDOW, 0, 0);
			//ManualSynchronization ends
		}
		wstring update_dir = sjl_path + L"\\update";
		wstring update_script_file_name = update_dir + L"\\update.script";
		if (is_file_exist(update_script_file_name))
		{
			debug("found update script");
			FILE *f;
			vector<wstring> updateLines;
			//ManualSynchronization on 24.12.23 07:36
			int fo_res = fopen_s(&f, to_string_(update_script_file_name).c_str(), "rt");
			if (!fo_res)
			{

				wchar_t buffer[4096];
				while (fgetws(buffer, sizeof(buffer), f))
				{
					trim_line(buffer);
					wstring line = buffer;
					updateLines.push_back(line);
				}
				fclose(f);
			}
			else
			{
				debug("unable to open update file, code = " + fo_res);
				throw_exception(L"unable to open update file");
			}
			//ManualSynchronization ends
			int mode = UPDATE_STATE_READING_TYPE;
			wstring sourceFile;
			for (int n = 0; n < updateLines.size(); n++)
			{
				wstring line = updateLines[n];
				if (mode == UPDATE_STATE_READING_TYPE)
				{
					if (line.compare(L"file-move:") == 0)
					{
						debug("current command is file-move");
						mode = UPDATE_STATE_READING_MOVE_SOURCE;
						continue;
					}
					if (line.compare(L"file-delete:") == 0)
					{
						debug("current command is file-move");
						mode = UPDATE_STATE_READING_DELETE_FILE;
						continue;
					}
					if (line.compare(L"sleep:") == 0)
					{
						debug("current command is sleep");
						mode = UPDATE_STATE_READING_SLEEP_TIMEOUT;
						continue;
					}
					if (line.compare(L"show-splash:") == 0)
					{
						debug("current command is show splash");
						mode = UPDATE_STATE_READING_SPLASH_FILE;
						continue;
					}
					if (line.compare(L"hide-splash:") == 0)
					{
						debug("current command is hide splash");
						hide_current_splash();
						mode = UPDATE_STATE_READING_TYPE;
						continue;
					}
					debug(L"undefined update mode " + line);
					throw_exception(L"undefined mode " + line);
				}
				if (mode == UPDATE_STATE_READING_DELETE_FILE)
				{
					debug("mode is file-delete");
					if (is_file_exist(line))
					{
						debug(L"file exists :" + line + L" trying to delete it");
						if (is_directory(line))
						{
							debug("deleting directory");
							remove_directory(line);
						}
						else if (!DeleteFileW(line.c_str()))
						{
							debug("unable to delete file");
							throw_exception(L"unable to delete file: " + line);
						}
					}
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
				if (mode == UPDATE_STATE_READING_SPLASH_FILE)
				{
					debug(L"splash file is set to " + line);
					show_splash(line);
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
				if (mode == UPDATE_STATE_READING_MOVE_SOURCE)
				{
					sourceFile = line;
					debug(L"source file is set to " + line);
					debug("mode changed to UPDATE_STATE_READING_MOVE_TARGET");
					mode = UPDATE_STATE_READING_MOVE_TARGET;
					continue;
				}
				if (mode == UPDATE_STATE_READING_MOVE_TARGET)
				{
					wstring target_file = line;
					debug(L"copying " + sourceFile + L" to " + target_file);
					if (is_file_exist(target_file))
					{
						debug(L"target file exists, trying to delete it");
						if (is_directory(target_file))
						{
							debug("deleting direcotry");
							remove_directory(target_file);
						}
						else if (!DeleteFileW(target_file.c_str()))
						{
							debug(L"unable to delete " + target_file);
							throw_exception(L"unable to delete " + target_file);
						}
					}
					if (is_directory(sourceFile))
					{
						debug("copy directory");
						copy_directory(sourceFile, target_file);
					}
					else if (!CopyFileW(sourceFile.c_str(), target_file.c_str(), false))
					{
						debug(L"unable to copy " + sourceFile + L" to " + target_file);
						throw_exception(L"unable to copy " + sourceFile + L" to " + target_file);
					}
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
				if (mode == UPDATE_STATE_READING_SLEEP_TIMEOUT)
				{
					int sleep = stoi(line);
					debug(L"sleep timeout: " + line);
					Sleep(sleep);
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
			}
			debug(L"removing update directory " + update_dir);
			remove_directory(update_dir);
		}

		vector<string> vmOptionLines;
		//ManualSynchronization on 18.01.24 23:24
		//string path = get_string_param(IDS_VM_OPTIONS_FILE);
		string path = get_resourcestring_param("VM_OPTIONS_FILE");
		if (!path.empty())
		{
			debug("reading options file " + path);
			//ManualSynchronization on 24.12.23 07:35
			FILE *f;
			if (is_file_exist(to_wstring_(path)))
			{
				debug("file exists");
				int res = fopen_s(&f, path.c_str(), "rt");
				if (!res)
				{

					char buffer[4096];
					while (fgets(buffer, sizeof(buffer), f))
					{
						trim_line(buffer);
						if (strlen(buffer) > 0 && buffer[0] != '#' && strcmp(buffer, "-server") != 0)
						{
							string line(buffer);
							debug("added options line " + line);
							vmOptionLines.push_back(line);
						}
					}
					fclose(f);
				}
				else
				{
					//ManualSynchronization on 23.12.23 14:34 - error 4 ( to_wstring not working with integer )
					//ManualSynchronization on 24.12.23 07:35
					debug(L"unable to open file, code " + std::to_wstring(res));
					throw_exception(L"unable to open options file " + to_wstring_(path));
					//ManualSynchronization
				}
			}
			//ManualSynchronization ends
		}

		//ManualSynchronization on 18.01.24 23:25
		//string predefinedOptions = get_string_param(IDS_VM_OPTIONS);
		string predefinedOptions = get_resourcestring_param("VM_OPTIONS");
		if (!predefinedOptions.empty())
		{
			debug("predefined options exist");
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
		//ManualSynchronization starts on 10.01.24 14:54
		//activate_window();
		wstring binDir = jvm_path + L"\\bin";
		wstring dllName = binDir + L"\\server\\jvm.dll";
		//ManualSynchronization on 10.01.24 21:10
		//dllName =L"jvm.dll";
		if (!is_file_exist(dllName))
		{
			debug(L"file does not exist " + dllName);
			dllName = binDir + L"\\client\\jvm.dll";
			if (!is_file_exist(dllName))
			{
				debug(L"file does not exist " + dllName);
				debug(L"unable to locate jvm.dll in " + binDir);
				throw_exception(L"unable to locate jvm.dll");
			}
		}

		SetEnvironmentVariableW(L"JAVA_HOME", binDir.c_str());
		debug(L"JAVA_HOME is set to " + binDir);
		//ManualSynchronization on 23.12.23 14:34 - error 6
		SetDllDirectoryW(nullptr);
		SetCurrentDirectoryW(base_path.c_str());
		debug(L"current directory is set to " + base_path);
		HMODULE hJVM = LoadLibraryW(dllName.c_str());
		if (!hJVM)
		{
			debug(L"unable to load jvm from " + dllName);
			debug_args(L"unable to load jvm from: %s lastError: %d", dllName.c_str(), GetLastError());
			throw_exception(L"unable to load jvm from " + dllName);
		}
		JNI_createJavaVM pCreateJavaVM = NULL;
		if (hJVM)
		{
			pCreateJavaVM = (JNI_createJavaVM)GetProcAddress(hJVM, "JNI_CreateJavaVM");
		}
		if (!pCreateJavaVM)
		{
			debug("unable to find address of JNI_CreateJavaVM");
			throw_exception(L"unable to find address of JNI_CreateJavaVM");
		}
		int vmOptionsCount = 2 + vmOptionLines.size();
		JavaVMOption *vmOptions = (JavaVMOption *)calloc(vmOptionsCount, sizeof(JavaVMOption));
		vmOptions[0].optionString = (char *)"exit";
		vmOptions[0].extraInfo = (void *)jniExitHook;
		//ManualSynchronization on 18.01.24 23:26
		//string cp = get_string_param(IDS_CLASS_PATH);
		string cp = get_resourcestring_param("CLASS_PATH");
		if (cp.empty())
		{
			debug("classpath is not defined");
			throw_exception(L"classpath is not defined");
		}
		string cpOption = "-Djava.class.path=" + cp;
		debug("class path is set to " + cp);
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

		//ManualSynchronization on 19.01.24 00:47
		//EnumDesktopWindows(NULL, EnumWindowsProc_JVM, 0);
		//HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
		////EnumChildWindows(GetDesktopWindow(), EnumWindowsProc_JVM, 0);
		HANDLE hThread = CreateThread(NULL, 0, EnumerateWindowsThread, NULL, 0, NULL);

		//if (hThread != NULL) {
		//	// Optionally, you can wait for the thread to complete
		//	WaitForSingleObject(hThread, INFINITE);

		//	// Close the thread handle when you're done with it
		//	CloseHandle(hThread);
		//}
		//g_hook = SetWindowsHookEx(WH_MAX, CBTProc, NULL, GetCurrentThreadId());
		

		//MessageBox(0, L"Starting to createJAVAVM", 0, 0);
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
			debug("unablet to create jvm");
			throw_exception(L"unable to create jvm");
		}
		debug("jvm was created");
		jthrowable jtExcptn;
		//ManualSynchronization on 18.01.24 23:26
		//const std::string mainClassName = get_string_param(IDS_MAIN_CLASS);
		const std::string mainClassName = get_resourcestring_param("MAIN_CLASS");
		debug("main class is " + mainClassName);
		jclass mainClass = jenv->FindClass(mainClassName.c_str());
		if (!mainClass)
		{
			debug("unable to fine main class " + mainClassName);
			throw_exception(L"unable to find main class ");
		}

		jmethodID mainMethod = jenv->GetStaticMethodID(mainClass, "main", "([Ljava/lang/String;)V");
		if (!mainMethod)
		{
			debug("unable to fine main method");
			throw_exception(L"unable to find main method ");
		}
		jclass stringClass = jenv->FindClass("java/lang/String");
		jobjectArray args = jenv->NewObjectArray(0, stringClass, NULL);
		//ManualSynchronization on 19.01.24 00:04
		

		
		//hide_current_splash();
		//ShowWindow(main_app_window_handle, SW_HIDE);

		//ManualSynchronization ends
		
		jenv->CallStaticVoidMethod(mainClass, mainMethod, args);
		jthrowable exc = jenv->ExceptionOccurred();
		if (exc)
		{
			debug("error occurred invoking main method");
			throw_exception(L"Error invoking main method");
		}
		
		//ManualSynchronization 12.01.24 00:05
		//hide_current_splash();
		//ShowWindow(main_app_window_handle, SW_HIDE);
		//ManualSynchronization ends
		//activate_window();
	    jvm->DestroyJavaVM();
		debug("jvm was destroyed");
		////ManualSynchronization ends
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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
	//ManualSynchronization on 11.01.24 20:27
/*	MessageBoxW(NULL, exception_message.empty() ? L"Unknown error" : exception_message.c_str(),
				L"Error occurred", MB_OK);
	*/MessageBoxW(NULL, exception_message.empty() ? L"Unknown error" : exception_message.c_str(),
		L"Error occurred", MB_OK |MB_DEFAULT_DESKTOP_ONLY);

	
	//ManualSynchronization ends
}
//ManualSynchronization on 24.12.23 20:25
void debug_args(wchar_t* format, ...)
{
	if (debug_flag)
	{

		va_list args;
		va_start(args, format);

		wchar_t buf[1024] = { 0, };
		_vsnwprintf(buf, sizeof(buf), format, args);
		va_end(args);

		//ManualSynchronization on 10.01.24 12:11
		wchar_t temp_fileNameASCII[256];

		//ManualSynchronization on 10.01.24 12:21
		string output_file_name = "debug_output.log";//to_string_(sjl_path);
		//output_file_name += "debug_output.log";
		/*char temp_file_name[256] = "debug_output.log";
		*///ManualSynchronization ends
		wchar_t temp_time_data[256] = { 0 };
		wchar_t	new_line_str[256] = L"\r\n";
		struct tm* tm_ptr = NULL;
		time_t raw;

		time(&raw);
		tm_ptr = localtime(&raw);

		wcsftime(temp_time_data, 256, L"'%Y-%m-%d %H:%M:%S' ", tm_ptr);

		//ManualSynchronization on 10.01.24 12:22
/*		FILE* pFile = fopen(temp_file_name, "a");
	*/	FILE* pFile = fopen(output_file_name.c_str(), "a");
	//ManualSynchronization ends
	if (pFile)
	{
		fputws(new_line_str, pFile);
		fputws(temp_time_data, pFile);
		fputws(buf, pFile);
		fflush(pFile);
	}
		va_end(args);
	}
}
void debug_args(char* format, ...)
{
	if (debug_flag)
	{
		WCHAR buf[1024] = { 0 };
		va_list args;
		va_start(args, format);

		char buf2[2048] = { 0, };
		_vsnprintf(buf2, sizeof(buf2), format, args);
		va_end(args);

		MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024);

		//ManualSynchronization on 10.01.24 12:19
		string output_file_name = "debug_output.log";//to_string_(sjl_path);
		//output_file_name += "debug_output.log";
/*		char temp_file_name[256] = "debug_output.log";
		*///ManualSynchronization ends
		wchar_t temp_time_data[256] = { 0 };
		wchar_t	new_line_str[256] = L"\r\n";
		struct tm* tm_ptr = NULL;
		time_t raw;

		time(&raw);
		tm_ptr = localtime(&raw);

		wcsftime(temp_time_data, 256, L"'%Y-%m-%d %H:%M:%S' ", tm_ptr);

		//ManualSynchronization on 10.01.24 12:19
/*		FILE* pFile = fopen(temp_file_name, "a");
	*/	FILE* pFile = fopen(output_file_name.c_str(), "a");
		//ManualSynchronization ends
		fputws(new_line_str, pFile);
		fputws(temp_time_data, pFile);
		fputws(buf, pFile);
		fflush(pFile);

		va_end(args);
	}
}
void debug_(const char* format, ...)
{
	if (debug_flag)
	{
		WCHAR buf[1024] = { 0 };
		char  buf2[2048] = { 0 };

		va_list args;
		va_start(args, format);

		_vsnprintf(buf2, sizeof(buf2), format, args);
		va_end(args);
		//ManualSynchronization on 24.12.23 15:33
		int len = MultiByteToWideChar(CP_UTF8, 0, buf2, -1, 0, 0);
		wchar_t* wstr = (wchar_t*)calloc(sizeof(wchar_t), len + 2);
		MultiByteToWideChar(CP_UTF8, 0, buf2, -1, wstr, len + 2);
		std::wstring nresultwstr = wstr;
		free(wstr);
		//MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024);
		//ManualSynchronization on 24.12.23 15:38
		//ManualSynchronization starts on 24.12.23 15:43
		//int len = MultiByteToWideChar(CP_ACP, 0, buf2, -1, 0, 0);
		//char* str = (char*)calloc(sizeof(char*), len + 2);
		//MultiByteToWideChar(CP_ACP, 0, buf2, -1, str, len + 2);
		//ManualSynchronization ends
		//ManualSynchronization starts on 24.15.23 15:08
		////manualSynchronization on 24.12.23 15:05
		//time_t* tv_sec=NULL;
		////ManualSynchronization on 24.12.23 14:59
		//struct tm systime = { 0 };
		//gmtime_s(systime, tv_sec);
		SYSTEMTIME systime2;
		GetSystemTime(&systime2);
		//ManualSynchronization starts on 24.12.23 15:12
		char buftime[256];
		sprintf(buftime, "[%04d-%02d-%02d %02d:%02d]", systime2.wYear, systime2.wMonth, systime2.wDay, systime2.wHour,
			systime2.wMinute);
		//ManualSynchronization starts on 24.12.23 18:08

		len = MultiByteToWideChar(CP_UTF8, 0, buftime, -1, 0, 0);
		wstr = (wchar_t*)calloc(sizeof(wchar_t), len + 2);
		MultiByteToWideChar(CP_UTF8, 0, buftime, -1, wstr, len + 2);
		std::wstring nresulttimewstr = wstr; //L"[";
		//nresulttimewstr +=wstr;
		nresulttimewstr += L" ";
		free(wstr);

		//ManualSynchroniaztion starts on 24.12.23 15:46
		wchar_t wbuftime[256] = { 0 };
		wsprintf(wbuftime, L"%04d-%02d-%02d %02d:%02d.%07d", systime2.wYear, systime2.wMonth, systime2.wDay, systime2.wHour,
			systime2.wMinute, systime2.wSecond, systime2.wMilliseconds * 10000);

		//ManualSynchronization ends
		//ManualSynchronization starts on 24.12.23 15:10

		//char buftime[256];
		//sprintf(buftime, "%04d-%02d-%02dT%02d:%02d.%07dZ", 1900 + systime.tm_year, systime.tm_mon + 1, systime.tm_mday, systime.tm_hour,
		//	systime.tm_min, systime.tm_sec, (int)(tv_usec * 10));
		//ManualSynchronization ends

		//ManualSynchronization on 24.12.23 14:41
		//wprintf(L"%s", buf);
		//ManualSynchronization starts on 24.12.23 14:56
		//DWORD nmBytesWritten;
		DWORD nmBytesWritten = 0;
		LPOVERLAPPED lpOverlapped;
		DWORD Access = GENERIC_WRITE;
		DWORD WinFlags = FILE_SHARE_WRITE;
		DWORD CREATE = CREATE_ALWAYS;
		HANDLE hFile = CreateFileA("debug_output.txt", Access, WinFlags, NULL, CREATE,
			FILE_ATTRIBUTE_NORMAL, NULL);
		//ManualSynchronization on 24.12.23 17:44
		//
		////ManualSynchronization on 24.12.23 14:52
		////WriteFile(hFile, buf2, sizeof(buf2), &nmBytesWritten, NULL);
		////ManualSynchronization on 24.12.23 15:15
		////strcat(buftime,buf2 );
		////ManualSynchronization on 24.12.23 15:20
		//std::string outputstr = "[";
		//outputstr += buftime;
		//outputstr += "] ";
		////ManualSynchronization on 24.12.23 15:37
		////ManualSynchronization on 24.12.23 15:45
		////outputstr += nresultwstr;
		////outputstr += buf2;

		////ManualSynchronization on 24.12.23 15:21
		////WriteFile(hFile, buf2, strlen(buf2), &nmBytesWritten, NULL);
		////ManualSynchronization on 24.12.23 15:43
		////WriteFile(hFile, outputstr.data(), outputstr.size(), &nmBytesWritten, NULL);
		////ManualSynchronization starts on 24.12.23 15:48
		////ManualSynchronization starts on 24.12.23 16:47
		//std::wstring woutputstr = L"[";
		//woutputstr += wbuftime;
		//woutputstr += L"] ";
		//woutputstr += nresultwstr;
		////ManualSynchronization ends
		////ManualSynchronization on 24.12.23 16:35
		//std::string strbuftime(buftime,strlen(buftime));
		//std::wstring wstrbuftime(strbuftime.begin(), strbuftime.end());
		////ManualSynchronization on 24.12.23 16:44
		////std::wstring woutputstr;
		////woutputstr = L"["+ wstrbuftime+L"]" + nresultwstr;
		////WriteFile(hFile, woutputstr.c_str(), woutputstr.size(), &nmBytesWritten, NULL);
		////ManualSynchronization on 24.12.23 16:36
		////WriteFile(hFile, wstrbuftime.c_str(), wstrbuftime.size(), &nmBytesWritten, NULL);
		////WriteFile(hFile, nresultwstr.data(), nresultwstr.size(), &nmBytesWritten, NULL);
		//WriteFile(hFile, woutputstr.c_str(), woutputstr.size(), &nmBytesWritten, NULL);
		//CloseHandle(hFile);
		//ManualSynchronization starts on 24.12.23 17:57
		//wchar_t* timewidestring = L"24.12.23 17:46 ";
		//WriteFile(hFile, timewidestring, wcslen(timewidestring)*2, &nmBytesWritten, NULL);
		//ManualSynchronization ends

		WriteFile(hFile, nresulttimewstr.c_str(), nresulttimewstr.size() * 2, &nmBytesWritten, NULL);
		WriteFile(hFile, nresultwstr.c_str(), nresultwstr.size(), &nmBytesWritten, NULL);
		CloseHandle(hFile);
		va_end(args);
	}
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

void trim_line(wchar_t *line)
{
	wchar_t *p = line + wcslen(line) - 1;
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
	debug("analyzing predefined option " + token);
	int markerPos = token.find("??");
	if (markerPos != string::npos)
	{
		debug("marker ?? exists");
		bool found = false;
		string start = token.substr(0, markerPos);
		for (int n = 0; n < vmOptionLines.size(); n++)
		{
			if (vmOptionLines[n].find(start) == 0)
			{
				debug("found existing option " + vmOptionLines[n]);
				found = true;
				break;
			}
		}
		if (!found)
		{
			string option = token.substr(0, markerPos) + token.substr(markerPos + 2);
			debug("adding option " + option);
			vmOptionLines.push_back(option);
		}
	}
	else
	{
		debug("option added " + token);
		vmOptionLines.push_back(token);
	}
}

void debug(wstring message)
{
	/*
	VA_ARGS -wikipedia , chatgtp
	ARG_LIST - wikipedia, chatgpt

	*/
	if (debug_flag)
	{
		wstring result = message + L"\n";
		DWORD dwBytesWritten = 0;
		//ManualSynchronization on 24.12.23 07:34
		string strBuf = to_string_(result);
		WriteFile(log_file, strBuf.c_str(), strBuf.size(), &dwBytesWritten,
				  NULL);
		//ManualSynchronization ends
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
//ManualSynchronization starts on 24.12.23 06:21
string to_string_(const wstring &str)
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
//ManualSynchronization ends
void init_paths()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	wstring pathStr = path;
	executable_path = path;
	size_t position = pathStr.find_last_of(L"\\");
	base_path = pathStr.substr(0, position);
	debug(L"base path is " + base_path);
	//ManualSynchronization on 18.01.24 23:19
	//jvm_path = get_wstring_param(IDS_JVM_PATH);
	jvm_path = get_resourcestring_paramW(L"JVM_PATH");
	debug(L"jvm path is " + jvm_path);
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
		log_file = init_utf8_file(log_file_name);
	}
}

bool is_directory(wstring path)
{
	DWORD attrib = GetFileAttributesW(path.c_str());

	return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0
		//ManualSynchronization starts on 10.01.24 22:30
		//&& (GetLastError() != ERROR_FILE_NOT_FOUND);
		;
		//ManualSynchronization ends
}

bool is_file_exist(wstring path)
{
	LPCWSTR szPath = path.c_str();
	DWORD dwAttrib = GetFileAttributesW(szPath);

	return dwAttrib != INVALID_FILE_ATTRIBUTES
		//ManualSynchronization starts on 10.01.24 22:29
		//&& (GetLastError() != ERROR_FILE_NOT_FOUND);
		;
		//ManualSynchronization ends
}

HANDLE init_utf8_file(wstring file_name)
{
	bool exists = is_file_exist(file_name);

	HANDLE handle = exists ? CreateFileW(file_name.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
										 OPEN_EXISTING, 0, NULL)
						   : CreateFileW(file_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
										 CREATE_ALWAYS, 0, NULL);
	if (!exists)
	{
		unsigned char smarker[3];
		DWORD bytesWritten;

		smarker[0] = 0xEF;
		smarker[1] = 0xBB;
		smarker[2] = 0xBF;
		WriteFile(handle, smarker, 3, &bytesWritten, NULL);
	}
	else
	{
		SetFilePointer(handle, 0l, nullptr, FILE_END);
	}
	return handle;
}

void init_sjl_dir()
{
	//ManualSynchronization on 18.01.24 23:20
	//sjl_path = base_path + L"\\" + get_wstring_param(IDS_SJL_DIR);
	sjl_path =	base_path + L"\\" + get_resourcestring_paramW(L"SJL_DIR");
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
	//ManualSynchronization on 18.01.24 23:21
	//wstring mutexName = get_wstring_param(IDS_MUTEX_NAME);
	wstring mutexName = get_resourcestring_paramW(L"MUTEX_NAME");
	if (!mutexName.empty())
	{
		SECURITY_ATTRIBUTES security;
		security.nLength = sizeof(SECURITY_ATTRIBUTES);
		security.bInheritHandle = TRUE;
		security.lpSecurityDescriptor = NULL;
		mutex = CreateMutexW(&security, FALSE, mutexName.c_str());

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
	//ManualSynchronization on 24.12.23 07:31
	return len ? to_string_(wstring(buf, len)) : "";
	return "";
	//ManualSynchronization
}
//ManualSynchronization on 18.01.24 22:54
wstring get_resourcestring_paramW(wchar_t* resourcename)
{
	if (g_debug_enabled) {
		

		// Switch on resourcename for known cases
		if (wcscmp(resourcename, L"MAIN_CLASS") == 0) {
			return std::wstring(L"com/gridnine/sjl/example/winGui/WinGui");
		}
		else if (wcscmp(resourcename, L"CLASS_PATH") == 0) {
			return std::wstring(L"..\\..\\win-gui\\dist\\sample-gui-app.jar");
		}
		else if (wcscmp(resourcename, L"JVM_PATH") == 0) {
			return std::wstring(L"..\\..\\win-gui\\dist\\jdk");
		}
		else if (wcscmp(resourcename, L"VM_OPTIONS") == 0) {
			return std::wstring(L"-Xms128m|-Xmx??256m");
		}
		else if (wcscmp(resourcename, L"VM_OPTIONS_FILE") == 0) {
			return std::wstring(L"win-gui.options");
		}
		else if (wcscmp(resourcename, L"RESTART_EXIT_CODE") == 0) {
			return std::wstring(L"79");
		}
		else if (wcscmp(resourcename, L"MUTEX_NAME") == 0) {
			return std::wstring(L"SJL");
		}
		else if (wcscmp(resourcename, L"SJL_DIR") == 0) {
			return std::wstring(L".sjl");
		}
		else if (wcscmp(resourcename, L"APP_TITLE") == 0) {
			return std::wstring(L"SimpleJavaLauncher");
		}
		else if (wcscmp(resourcename, L"SJL") == 0) {
			return std::wstring(L"SJLauncher");
		}
		else if (wcscmp(resourcename, L"LAUNCH_SINGLETON") == 0) {
			return std::wstring(L"1");
		}
		else if (wcscmp(resourcename, L"SHOW_SPLASH") == 0) {
			return std::wstring(L"1");
		}
		else {
			// Handle unknown resource names
			
			return std::wstring(L"");
		}
	}
	else {
		// Debug mode disabled, implement the original logic here
		// ...

		
	
	wchar_t szTMPBuffer[MAX_LOADSTRING];
	bool bFailLoad = LoadStringFromResourceW(hInst, resourcename, szTMPBuffer, MAX_LOADSTRING);
	return bFailLoad ? std::wstring(szTMPBuffer) : L"";
	}
}
//ManualSynchronization on 18.01.24 23:15
string get_resourcestring_param(char* resourcename)
{
	if (g_debug_enabled) {
	// Switch on resourcename for known cases
	if (strcmp(resourcename, "MAIN_CLASS") == 0) {
		return std::string("com/gridnine/sjl/example/winGui/WinGui");
	}
	else if (strcmp(resourcename, "CLASS_PATH") == 0) {
		return std::string("..\\..\\win-gui\\dist\\sample-gui-app.jar");
	}
	else if (strcmp(resourcename, "JVM_PATH") == 0) {
		return std::string("..\\..\\win-gui\\dist\\jdk");
	}
	else if (strcmp(resourcename, "VM_OPTIONS") == 0) {
		return std::string("-Xms128m|-Xmx??256m");
	}
	else if (strcmp(resourcename, "VM_OPTIONS_FILE") == 0) {
		return std::string("win-gui.options");
	}
	else if (strcmp(resourcename, "RESTART_EXIT_CODE") == 0) {
		return std::string("79");
	}
	else if (strcmp(resourcename, "MUTEX_NAME") == 0) {
		return std::string("SJL");
	}
	else if (strcmp(resourcename, "SJL_DIR") == 0) {
		return std::string(".sjl");
	}
	else if (strcmp(resourcename, "APP_TITLE") == 0) {
		return std::string("SimpleJavaLauncher");
	}
	else if (strcmp(resourcename, "SJL") == 0) {
		return std::string("SJLauncher");
	}
	else if (strcmp(resourcename, "LAUNCH_SINGLETON") == 0) {
		return std::string("1");
	}
	else if (strcmp(resourcename, "SHOW_SPLASH") == 0) {
		return std::string("1");
	}
	else {
		// Handle unknown resource names
		
		return std::string("");
	}
}
 else {
 
	
	char sTMPBuffer[MAX_LOADSTRING];
	bool bFailLoad = LoadStringFromResource(hInst, resourcename, sTMPBuffer, MAX_LOADSTRING);
	return bFailLoad ? std::string(sTMPBuffer) : "";
	}
}
void close_log_file_handle()
{
	if (log_file)
	{
		CloseHandle(log_file);
	}
}
//ManualSynchronization starts on 23.12.23 14:47 - error 4
wstring to_wstring_(string s)
{
	std::wstring ws(s.size(), L' '); // Overestimate number of code points.
	ws.resize(std::mbstowcs(&ws[0], s.c_str(), s.size()));
	return ws;
}
//ManualSynchronization ends
void remove_directory(wstring dir)
{
	dir.push_back(L'\0');
	SHFILEOPSTRUCTW file_op = {
		NULL,
		FO_DELETE,
		dir.c_str(),
		L"",
		FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
		false,
		0,
		L""};
	int result = SHFileOperationW(&file_op);
	if (result)
	{
		debug(L"unable to delete directory " + dir + L" code=" + std::to_wstring(result));
		throw_exception(L"unable to delete directory " + dir);
	}
}

void copy_directory(wstring source, wstring target)
{
	source.push_back(L'\0');
	target.push_back(L'\0');
	SHFILEOPSTRUCTW file_op = {
		NULL,
		FO_COPY,
		source.c_str(),
		target.c_str(),
		FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
		false,
		0,
		L""};
	int result = SHFileOperationW(&file_op);
	if (result)
	{
		auto pstr_test = std::to_string(result);
		debug(L"unable to copy " + source + L" to " + target + L" code=" );
		throw_exception(L"unable to copy " + source + L" to " + target);
	}
}

BOOL CALLBACK enumwndfn(HWND hwnd, LPARAM lParam)
{
	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);
	if (process_id == processId)
	{
		
		BringWindowToTop(hwnd);
		ShowWindow(hwnd, SW_NORMAL);
		SetForegroundWindow(hwnd);
		//ManualSynchronization 10.01.24 17:49 
		//task#3
		//return FALSE;
	}
	return TRUE;
}


DWORD WINAPI show_splash_function(LPVOID lpParam)
{
	WNDCLASSW wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = splash_window_procedure;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = SPLASH_WINDOW_CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	RegisterClassW(&wc);

	int x = (GetSystemMetrics(SM_CXSCREEN) - splash_bm.bmWidth) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - splash_bm.bmHeight) / 2;

	splash_window_handle = CreateWindowW(wc.lpszClassName, L"",
										 (WS_BORDER),
										 x, y, splash_bm.bmWidth, splash_bm.bmHeight, NULL, NULL, hInst, NULL);
	SetWindowLong(splash_window_handle, GWL_STYLE, 0); // remove all window styles, check MSDN for details

	ShowWindow(splash_window_handle, SW_SHOW);
	UpdateWindow(splash_window_handle);
	MSG msg;
	while (GetMessage(&msg, splash_window_handle, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_CLOSE)
		{
			break;
		}
	}
	debug("splash is hidden");
	return 0;
}
void show_splash(wstring image_path)
{
	//ManualSynchronization on 10.01.24 13:48
	h_splash_bitmap = (HBITMAP)LoadImageW(hInst, image_path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
/*	h_splash_bitmap = (HBITMAP)LoadImageA(hInst, "sample_24bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!h_splash_bitmap)
	{
		h_splash_bitmap = (HBITMAP)LoadImageA(hInst, "sample_24bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	*///ManualSynchronization ends
	if (!h_splash_bitmap)
	{
		debug(L"unable to load bitmap image from " + image_path);
		throw_exception(L"unable to load bitmap image from " + image_path);
	}
	GetObject(h_splash_bitmap, sizeof(BITMAP), &splash_bm);
	DWORD threadId;
	splash_screen_thread = CreateThread(
		NULL,
		0,
		show_splash_function,
		NULL,
		0,
		&threadId);
	debug(L"splash thread is created");
}

void hide_current_splash()
{
	debug("hiding splash");
	PostMessage(splash_window_handle, WM_CLOSE, 0, 0);
	WaitForSingleObject(splash_screen_thread, INFINITE);
	UnregisterClassW(SPLASH_WINDOW_CLASS_NAME, hInst);
	debug("splash screen is hidden");
}

LRESULT CALLBACK splash_window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		h_dc = GetDC(hWnd);
		h_dc_mem = CreateCompatibleDC(h_dc);
		h_splash_old_bitmap = (HBITMAP)SelectObject(h_dc_mem, h_splash_bitmap);
		ReleaseDC(hWnd, h_dc);
		return 0;
	case WM_PAINT:
		h_dc = BeginPaint(hWnd, &splash_ps);

		GetClientRect(hWnd, &splash_rect);
		SetStretchBltMode(h_dc, STRETCH_HALFTONE);
		StretchBlt(h_dc, 0, 0, splash_rect.right, splash_rect.bottom,
				   h_dc_mem, 0, 0, splash_bm.bmWidth, splash_bm.bmHeight, SRCCOPY);

		EndPaint(hWnd, &splash_ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		DeleteDC(h_dc_mem);
		DeleteObject(h_splash_bitmap);
		DeleteObject(h_splash_old_bitmap);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI activate_window_function(LPVOID lpParam)
{
	Sleep(50);
	EnumWindows(enumwndfn, 1);
	return false;
}

void activate_window()
{
	DWORD threadId;
	CreateThread(
		NULL,
		0,
		activate_window_function,
		NULL,
		0,
		&threadId);
}

HANDLE CheckOneInstanceLaunched()
{
	wstring mutexName = L"Global\\";
	//ManualSynchronization on 18.01.24 23:22
	//mutexName+=get_wstring_param(IDS_MUTEX_NAME);
	mutexName+= get_resourcestring_paramW(L"MUTEX_NAME");
	HANDLE handle = NULL;

	//ManualSynchronization on 10.01.24 17:45
/*	handle = CreateEventW(0, 0, 0, mutexName.c_str());
	if (handle && GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(handle);
		handle = NULL;
		return NULL;
	}
	if (!handle)
		handle = INVALID_HANDLE_VALUE;
	*/
	handle = OpenMutexW(MUTEX_MODIFY_STATE, FALSE, mutexName.c_str());
	if (handle)
	{
		CloseHandle(handle);
		return NULL;
	}
	else
	{
		handle = CreateMutexW(NULL, FALSE, mutexName.c_str());
		if (handle == NULL)
			return NULL;
	}
	//ManualSynchronization ends
	return handle;
}
//ManuaSynchronization starts on 18.01.24 21:03
int LoadStringFromResourceW(HINSTANCE hInstance, LPWSTR ResourceName, LPWSTR Buffer, int cchBufferMax)
{
	HRSRC hResource = FindResource(hInstance, ResourceName, RT_RCDATA);
	if (hResource == nullptr) {
		// Resource not found
		return false;
	}

	HGLOBAL hGlobal = LoadResource(hInstance, hResource);
	if (hGlobal == nullptr) {
		// Failed to load resource
		return false;
	}

	LPVOID lpResourceData = LockResource(hGlobal);
	if (lpResourceData == nullptr) {
		// Failed to lock resource
		FreeResource(hGlobal);
		return false;
	}

	int resourceSize = SizeofResource(hInstance, hResource);

	// Ensure the buffer is large enough
	if (resourceSize + 1 > cchBufferMax) {
		// Buffer too small
		FreeResource(hGlobal);
		return false;
	}
	//ManualSynchronization starts on 18.01.24 21:59
	/*int len = MultiByteToWideChar(CP_UTF8, 0, static_cast<char*>(lpResourceData), -1, 0, 0);
	wchar_t* wstr = (wchar_t*)calloc(sizeof(wchar_t), len + 2);
	MultiByteToWideChar(CP_UTF8, 0, static_cast<char*>(lpResourceData), -1, wstr, len + 2);*/

	// ManualSynchronization ends
	// Copy the resource data into the buffer
	//ManualSynchronization on 18.01.24 22:01
	//wcsncpy_s(Buffer, cchBufferMax, wstr, len);
	//wcsncpy_s(Buffer, cchBufferMax, static_cast<LPCWSTR>(lpResourceData), len);
	
	 // Convert the multi-byte string to wide characters (UTF-8 assumed)
	int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, static_cast<LPCCH>(lpResourceData), resourceSize, Buffer, cchBufferMax);
	if (wideCharSize == 0) {
		// Conversion failed
		FreeResource(hGlobal);
		return false;
	}
	// Null-terminate the string
	Buffer[resourceSize] = L'\0';

	// Release the resource
	FreeResource(hGlobal);

	return true;
}
//ManuaSynchronization ends

//ManualSynchronization on 18.01.24 23:03
int LoadStringFromResource(HINSTANCE hInstance, char* ResourceName, char* Buffer, int cchBufferMax)
{
	HRSRC hResource = FindResourceA(hInstance, ResourceName, MAKEINTRESOURCEA(10));
	if (hResource == nullptr) {
		// Resource not found
		return false;
	}

	HGLOBAL hGlobal = LoadResource(hInstance, hResource);
	if (hGlobal == nullptr) {
		// Failed to load resource
		return false;
	}

	LPVOID lpResourceData = LockResource(hGlobal);
	if (lpResourceData == nullptr) {
		// Failed to lock resource
		FreeResource(hGlobal);
		return false;
	}

	int resourceSize = SizeofResource(hInstance, hResource);

	// Ensure the buffer is large enough
	if (resourceSize + 1 > cchBufferMax) {
		// Buffer too small
		FreeResource(hGlobal);
		return false;
	}
	//ManualSynchronization starts on 18.01.24 21:59
	/*int len = MultiByteToWideChar(CP_UTF8, 0, static_cast<char*>(lpResourceData), -1, 0, 0);
	wchar_t* wstr = (wchar_t*)calloc(sizeof(wchar_t), len + 2);
	MultiByteToWideChar(CP_UTF8, 0, static_cast<char*>(lpResourceData), -1, wstr, len + 2);*/

	// ManualSynchronization ends
	// Copy the resource data into the buffer
	//ManualSynchronization on 18.01.24 22:01
	//wcsncpy_s(Buffer, cchBufferMax, wstr, len);
	//strcpy_s(Buffer, resourceSize, static_cast<char*>(lpResourceData) );
	strcpy(Buffer, static_cast<char*>(lpResourceData));
	
	
	// Null-terminate the string
	Buffer[resourceSize] = '\0';

	// Release the resource
	FreeResource(hGlobal);

	return true;
}
//ManuaSynchronization ends
//ManualSynchronization ends
void InitResourceData()
{
	//ManualSynchronization on 17.01.24 17:26
	// 


	//LPCTSTR lpResourceName = L"CUSTOMDATA";

	//LPCTSTR lpResourceType = RT_RCDATA;
	//// Find the specified resource
	//HRSRC hResource = FindResource(hInst, lpResourceName, lpResourceType);
	//if (hResource == NULL) {
	//	// Resource not found
	//	MessageBox(0, L"Resource Not Found", 0, 0);
	//	return;
	//}
	//// Load the resource
	//HGLOBAL hGlobal = LoadResource(hInst, hResource);
	//if (hGlobal != NULL) {
	//	// Lock the resource to obtain a pointer to the raw data
	//	LPVOID pData = LockResource(hGlobal);
	//	if (pData != NULL) {
	//		// Assuming the manifest is a null-terminated string (which it typically is)
	//		LPCSTR manifestString = reinterpret_cast<LPCSTR>(pData);

	//		// Print or use the manifest string
	//		MessageBoxA(0, manifestString, "Manifest Content:\n" ,0);
	//	}
	//	else {
	//		
	//	}
	//}
	//else {
	//	MessageBox(0, L"Failed to load Resource", 0, 0);
	//}

	
	//ManualSynchronization on 11.01.24 17:46
	//LoadStringW(hInst, titleid, lpbuffer, lpbuffermax);
	//LoadStringW(hInst, classid, lpbuffer, lpbuffermax);
	//LoadStringW(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadStringW(hInst, IDC_SJL, szWindowClass, MAX_LOADSTRING);
	//ManualSynchronization ends

	//ManualSynchronization on 11.01.24 21:01
	wchar_t szTMPBuffer[MAX_LOADSTRING];
	//LoadStringW(hInst, IDS_LAUNCH_SINGLETON, szTMPBuffer, MAX_LOADSTRING);
	//launch_singleton_flag = _wtoi(szTMPBuffer);

	//ManualSynchronization on 11.01.24 23:32
	//LoadStringW(hInst, IDS_SHOW_SPLASH, szTMPBuffer, MAX_LOADSTRING);
	//g_bShow_splash_flag =		_wtoi(szTMPBuffer);

	//ManualSynchronization on 17.01.24 20:38
	//LoadStringW(hInst, 1017, szTMPBuffer, MAX_LOADSTRING);
	//MessageBoxW(0, szTMPBuffer, 0, 0);
	//ManualSynchronization on 18.01.24 21:19
	//LoadStringFromResourceW(hInst, L"CUSTOMDATA", szTMPBuffer, MAX_LOADSTRING);
	//MessageBoxW(0, szTMPBuffer, 0, 0);

	//ManualSynchronization on 18.01.24 21:47
	//LoadStringFromResourceW(hInst, L"APP_TITLE", szTitle, MAX_LOADSTRING);
	std::wstring TMPwstring = get_resourcestring_paramW(L"APP_TITLE");
	wcscpy_s(szTitle, TMPwstring.c_str());

	//LoadStringFromResourceW(hInst, L"SJL", szWindowClass, MAX_LOADSTRING);
	TMPwstring = get_resourcestring_paramW(L"SJL");
	wcscpy_s(szWindowClass, TMPwstring.c_str());
	//MessageBoxW(0, szTitle, 0, 0);
	//MessageBoxW(0, szWindowClass, 0, 0);

	//ManualSynchronization on 18.01.24 22:51
	//LoadStringFromResourceW(hInst, L"LAUNCH_SINGLETON", szTMPBuffer, MAX_LOADSTRING);
	TMPwstring = get_resourcestring_paramW(L"LAUNCH_SINGLETON");
	//launch_singleton_flag = _wtoi(szTMPBuffer);
	launch_singleton_flag = _wtoi(TMPwstring.c_str());
	//LoadStringFromResourceW(hInst, L"SHOW_SPLASH", szTMPBuffer, MAX_LOADSTRING);
	TMPwstring = get_resourcestring_paramW(L"SHOW_SPLASH");
	g_bShow_splash_flag = _wtoi(TMPwstring.c_str());
	//g_bShow_splash_flag = _wtoi(szTMPBuffer);

	//ManualSynchronization on 18.01.24 22:58
	//wstring m_swApptitle = get_resourcestring_paramW(L"APP_TITLE");
	//MessageBoxW(0, m_swApptitle.c_str(), 0, 0);
	//ManualSynchronization on 18.01.24 23:10
	//char sTMPBuffer[MAX_LOADSTRING];
	//string m_sApptitle = get_resourcestring_param("APP_TITLE");
	//LoadStringFromResource(hInst, "APP_TITLE", sTMPBuffer, MAX_LOADSTRING);
	//MessageBoxA(0, sTMPBuffer, 0, 0);
	//MessageBoxA(0, m_sApptitle.c_str(), 0, 0);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	// Transparent color (R, G, B, A)
	COLORREF transparentColor = RGB(0, 0, 0);
	HBRUSH hbrTransparent = CreateSolidBrush(transparentColor);

	WNDCLASSEXW wcex;

	wcex.cbSize					= sizeof(WNDCLASSEXW);
	wcex.style					= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc			= WndProc;
	wcex.cbClsExtra				= 0;
	wcex.cbWndExtra				= 0;
	wcex.hInstance				= hInstance;
	wcex.hIcon					= LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SJL));
	wcex.hCursor				= LoadCursor(nullptr,IDC_ARROW);
	wcex.hbrBackground			= hbrTransparent;
	wcex.lpszMenuName			= MAKEINTRESOURCE(IDC_SJL);
	wcex.lpszClassName			= szWindowClass;
	wcex.hIconSm				= LoadIcon(wcex.hInstance,MAKEINTRESOURCE(IDI_SMALL));
	
	return RegisterClassExW(&wcex);
}
#pragma optimize("", off)

BOOL InitInstance(HINSTANCE hInstance, int cmd)
{
	HWND hwnd = CreateWindowExW(WS_EX_LAYERED,szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT,300,100, nullptr, nullptr, hInstance, nullptr);
	if (!hwnd)
		return 0;
	SetWindowLong(hwnd, GWL_STYLE, 0);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	main_app_window_handle = hwnd;

	return 0;
}
#pragma optimize("", on)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmID = LOWORD(wParam);
		switch (wmID)
		{

		default:
			return DefWindowProc(hWnd,message,wParam,lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}