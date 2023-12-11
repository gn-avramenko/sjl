#include "wincommon.h"

using namespace std;

bool debug_flag = false;
HANDLE log_file;
wstring base_path;
wstring sjl_path;
wstring exception_message;
BOOL wow64 = FALSE;
wstring jvm_path;

void set_debug_flag(bool value) {
	debug_flag = value;
}

string to_utf8(const wstring &s) {
	string utf8;
	int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.length(), NULL, 0,
	NULL, NULL);
	if (len > 0) {
		utf8.resize(len);
		WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.length(), &utf8[0], len,
		NULL, NULL);
	}
	return utf8;
}

void debug(wstring message) {
	if (debug_flag) {
		wstring result = message + L"\n";
		DWORD dwBytesWritten = 0;
		string strBuf = to_utf8(result);
		WriteFile(log_file, strBuf.c_str(), strBuf.size(), &dwBytesWritten,
		NULL);
	}
}

bool is_file_exist(wstring path) {
	LPCWSTR szPath = path.c_str();
	DWORD dwAttrib = GetFileAttributesW(szPath);

	return dwAttrib != INVALID_FILE_ATTRIBUTES;
}

void throw_exception(wstring message) {
	exception_message = message;
	throw std::exception();
}

HANDLE init_utf8_file(wstring file_name) {
	if (is_file_exist(file_name) && !DeleteFileW(file_name.c_str())) {
		throw_exception(L"unable to delete file " + file_name);
	}

	HANDLE handle = CreateFileW(file_name.c_str(), GENERIC_WRITE, 0, NULL,
	CREATE_ALWAYS, 0, NULL);
	unsigned char smarker[3];
	DWORD bytesWritten;

	smarker[0] = 0xEF;
	smarker[1] = 0xBB;
	smarker[2] = 0xBF;
	WriteFile(log_file, smarker, 3, &bytesWritten, NULL);
	return handle;
}

void init_sjl_dir() {
	sjl_path = base_path + L"\\.sjl";
	if (!is_file_exist(sjl_path) && !CreateDirectoryW(sjl_path.c_str(), NULL)) {
		throw_exception(L"unable to create directory " + sjl_path);
	}
}

void init_paths() {
	wchar_t path[MAX_PATH] { };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	wstring pathStr = path;
	size_t position = pathStr.find_last_of(L"\\");
	base_path = pathStr.substr(0, position);
	jvm_path = L"c:\\IdeaProjects\\sjl\\examples\\win-gui\\dist\\jdk";
}

void init_debug_file() {
	if (debug_flag) {
		init_sjl_dir();
		wstring log_file_name = sjl_path + L"\\sjl.log";
		if (is_file_exist(log_file_name)
				&& !DeleteFileW(log_file_name.c_str())) {
			throw_exception(L"unable to delete file " + log_file_name);
		}
		log_file = init_utf8_file(log_file_name);
	}
}

void setWow64Flag() {
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
			GetModuleHandleW(L"kernel32"), "IsWow64Process");

	if (fnIsWow64Process != NULL) {
		fnIsWow64Process(GetCurrentProcess(), &wow64);
	}

	debug(wow64? L"WOW64:\t\t yes": L"WOW64:\t\t no");
}

void createMutex() {
	wstring mutexName = L"sjl";

	if (mutexName.empty()) {
		SECURITY_ATTRIBUTES security;
		security.nLength = sizeof(SECURITY_ATTRIBUTES);
		security.bInheritHandle = TRUE;
		security.lpSecurityDescriptor = NULL;
		CreateMutexW(&security, FALSE, mutexName.c_str());

		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			debug(L"Instance already exists.");
			throw_exception(L"application instance is already launched");
		}
		debug(L"Create mutex:\t" + mutexName);
	}
}

void setWorkingDirectory() {
	_wchdir(base_path.c_str());
	debug(L"Working dir:\t" + base_path);
}

void close_log_file_handle() {
	if(log_file){
		CloseHandle(log_file);
	}
}

bool prepare(LPCSTR lpCmdLine) {
	try {
		string cmdLineStr = lpCmdLine;
		set_debug_flag(cmdLineStr.find("-debug") != string::npos);
		set_debug_flag(true);
		init_paths();
		init_debug_file();
		setWow64Flag();
		createMutex();
		setWorkingDirectory();
	} catch (...) {
		close_log_file_handle();
		return FALSE;
	}
	return TRUE;
}


wstring get_error_message() {
	return exception_message;
}

wstring get_jvm_path() {
	return jvm_path;
}
