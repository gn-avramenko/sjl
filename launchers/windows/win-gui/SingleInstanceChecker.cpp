#include "SingleInstanceChecker.h"
#include "Windows.h"

SingleInstanceChecker::SingleInstanceChecker(std::wstring aMutexName, std::wstring aAppTitle, Debug aDebug) {
	mutexName = aMutexName;
	appTitle = aAppTitle;
	debug = aDebug;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	EnumWindowsData* data = reinterpret_cast<EnumWindowsData*>(lParam);

	// Check if the window has the specified title
	wchar_t title[1024];
	if (GetWindowTextW(hwnd, title, sizeof(title)) > 0 && wcscmp(title, data->targetTitle) == 0)
	{
		data->hwnd = hwnd;
		return FALSE;
	}
	return TRUE;  // Continue enumerating
}

BOOL SingleInstanceChecker::Check() {
	if(mutexName.empty()) {
		debug.Log("mutex is empty, can continue");
		return true;
	}
	std::wstring mutexName = L"Global\\"+mutexName;
	HANDLE handle = OpenMutexW(MUTEX_MODIFY_STATE, FALSE, mutexName.c_str());
	if (!handle)
	{
		debug.Log("current handle is null");
		handle = CreateMutexW(NULL, FALSE, mutexName.c_str());
		if (handle == NULL) {
			debug.Log("handle can not be created");
			return FALSE;
		}
		debug.Log("handle was created");
		return TRUE;
	}
	EnumWindowsData data;
	data.targetTitle = appTitle.c_str();
	data.hwnd = nullptr;	

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));

	if (data.hwnd != nullptr) {
		// Bring the window to the foreground
		SetForegroundWindow(data.hwnd);
		debug.Log("running process was made foreground");
		return FALSE;
	}
	debug.Log(L"unable to find window with name %s", data.targetTitle);
	return TRUE;

}
