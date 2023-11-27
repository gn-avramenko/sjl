#include <windows.h>
#include <stdio.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <format>

using namespace std;

DWORD dwExitCode = 0;
PROCESS_INFORMATION pi;
#define ID_TIMER 1
HWND hWnd;
wofstream logFileStream;
boolean debugFlag = false;
#define debug(...) if(debugFlag){logFileStream << format(__VA_ARGS__) << L"\n";};
  
VOID CALLBACK TimerProc(
	HWND hwnd,			// handle of window for timer messages
	UINT uMsg,			// WM_TIMER message
	UINT_PTR idEvent,		// timer identifier
	DWORD dwTime) {		// current system time
	
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	if (dwExitCode != STILL_ACTIVE) {
		KillTimer(hWnd, ID_TIMER);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        if(debugFlag){
           logFileStream.close();
        }
        PostQuitMessage(0);
	}
}

void execute(HINSTANCE hInstance)
{
    STARTUPINFOW si;
    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    if (CreateProcessW(NULL, L"C:\\Users\\vova\\IdeaProjects\\sjl\\examples\\win-gui\\dist\\jdk\\bin\\javaw.exe -cp C:\\Users\\vova\\IdeaProjects\\sjl\\examples\\win-gui\\dist\\win-gui.jar com.gridnine.sjl.example.winGui.WinGui", NULL, NULL,
                      TRUE, 1000, NULL, NULL, &si, &pi))
    {
        hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", L"",
				WS_POPUP | SS_BITMAP,
				0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
        debug(L"process was created");
        if (!SetTimer (hWnd, ID_TIMER, 1000 /* 1s */, TimerProc)) {
            debug(L"timer started");
            dwExitCode = 11;
			return;
		}
    } else {
        debug(L"process was not created");
        dwExitCode=11;
        return;
    }
    MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    
    string cmd = lpCmdLine;
    debugFlag =cmd.find("-debug") != string::npos;
    if(debugFlag){
        wchar_t path[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, path, MAX_PATH);
        filesystem::path logFilePath = filesystem::path(path).parent_path();
        logFilePath /= L"sjl.log";
        wofstream logFileStream(logFilePath);       
    }
    execute( hInstance);
    return 0;
}

