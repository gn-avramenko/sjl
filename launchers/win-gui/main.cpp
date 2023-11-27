#include <windows.h>
#include <string>

using namespace std;

DWORD dwExitCode = -1;
PROCESS_INFORMATION pi;
#define ID_TIMER 1
HWND hWnd;
BOOL debugFlag = false;
HANDLE logFileStream;
wstring basePath;
wstring sjlDir;
#define OTHER_ERROR_CODE 10

VOID CALLBACK TimerProc(
    HWND hwnd,        // handle of window for timer messages
    UINT uMsg,        // WM_TIMER message
    UINT_PTR idEvent, // timer identifier
    DWORD dwTime)
{ // current system time

    GetExitCodeProcess(pi.hProcess, &dwExitCode);
    if (dwExitCode != STILL_ACTIVE)
    {
        KillTimer(hWnd, ID_TIMER);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        PostQuitMessage(0);
    }
}

void execute(HINSTANCE hInstance)
{
    STARTUPINFOW si;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags |= STARTF_USESTDHANDLES;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    si.cb = sizeof(STARTUPINFO);
    if (debugFlag)
    {
        HANDLE errorFile = CreateFileW((sjlDir + L"\\err.log").c_str(),
                                       FILE_APPEND_DATA,
                                       FILE_SHARE_WRITE | FILE_SHARE_READ,
                                       &sa,
                                       OPEN_ALWAYS,
                                       FILE_ATTRIBUTE_NORMAL,
                                       NULL);
        HANDLE outFile = CreateFileW((sjlDir + L"\\out.log").c_str(),
                                     FILE_APPEND_DATA,
                                     FILE_SHARE_WRITE | FILE_SHARE_READ,
                                     &sa,
                                     OPEN_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL);
        si.hStdError = errorFile;
        si.hStdOutput = outFile;
    }
        
   // wstring distPath = basePath;    
    wstring distPath = L"C:\\IdeaProjects\\sjl\\examples\\win-gui\\dist";
    wstring execPath = distPath;
    execPath += L"\\jdk\\bin\\javaw.exe";
    wstring cmdLine = execPath + L" -cp win-gui.jar com.gridnine.sjl.example.winGui.WinGui";
    if (CreateProcessW(NULL, &cmdLine[0], NULL, NULL,
                       TRUE, 1000, NULL, distPath.c_str(), &si, &pi))
    {
        hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", L"",
                               WS_POPUP | SS_BITMAP,
                               0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
        if (!SetTimer(hWnd, ID_TIMER, 1000 /* 1s */, TimerProc))
        {
            dwExitCode = OTHER_ERROR_CODE;
            return;
        }
    }
    else
    {
        dwExitCode = OTHER_ERROR_CODE;
        return;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

BOOL FileExists(wstring path)
{
    LPCWSTR szPath = path.c_str();
    DWORD dwAttrib = GetFileAttributesW(szPath);

    return dwAttrib != INVALID_FILE_ATTRIBUTES;
}

string to_utf8(const wstring &s)
{
    string utf8;
    int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.length(), NULL, 0, NULL, NULL);
    if (len > 0)
    {
        utf8.resize(len);
        WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.length(), &utf8[0], len, NULL, NULL);
    }
    return utf8;
}

void debug(wstring message)
{
    if (debugFlag)
    {
        wstring result = message + L"\n";
        DWORD dwBytesWritten = 0;
        string strBuf = to_utf8(result);
        WriteFile(logFileStream, strBuf.c_str(), strBuf.size(), &dwBytesWritten, NULL);
    }
}

void throwException(wstring message)
{
    ULONG_PTR lpArguments[1]{};
    lpArguments[0] = (ULONG_PTR)(message.c_str());
    RaiseException(OTHER_ERROR_CODE, EXCEPTION_NONCONTINUABLE, 1, lpArguments);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    string cmdLineStr = lpCmdLine;
    debugFlag = cmdLineStr.find("-debug") != string::npos;
    debugFlag = true;
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wstring pathStr = path;
    size_t position = pathStr.find_last_of(L"\\");
    basePath = pathStr.substr(0, position);
    sjlDir = basePath + L"\\.sjl";
    if (debugFlag)
    {
        if (!FileExists(sjlDir) && !CreateDirectoryW(sjlDir.c_str(), NULL))
            throwException(L"unable to create directory " + sjlDir);
        wstring logFileName = sjlDir + L"\\sjl.log";
        if (FileExists(logFileName) && !DeleteFileW(logFileName.c_str()))
            throwException(L"unable to delete file " + logFileName);
        logFileStream = CreateFileW(logFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        unsigned char smarker[3];
        DWORD bytesWritten;

        smarker[0] = 0xEF;
        smarker[1] = 0xBB;
        smarker[2] = 0xBF;
        WriteFile(logFileStream, smarker, 3, &bytesWritten, NULL);
        debug(L"launcher is running in debug mode");
    }

    execute( hInstance);
    if (debugFlag)
    {
        CloseHandle(logFileStream);
    }
    return dwExitCode;
}
