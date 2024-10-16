#include <vector>
#include <set>
#include <sstream>
#include <string>
#include "JVM.h"
#include "Utils.h"
#include "Windows.h"

DWORD dwExitCode = -1;
PROCESS_INFORMATION pi;
#define ID_TIMER 1
HWND hWnd;
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
		PostQuitMessage(dwExitCode);
	}
}


JVM::JVM(HINSTANCE* inst, ExceptionWrapper* ew, Locations* loc, Debug* deb, Resources* res, SingleInstanceChecker* aSic, SplashScreen* splash, std::string progParams, bool* nr)
{
	exceptionWrapper = ew;
	locations = loc;
	debug = deb;
	resources = res;
	sic = aSic;
	splashScreen = splash;
	programParams = progParams;	
	hInstance = inst;
	needRestart = nr;
}

void JVM::LaunchJVM() {
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
	SetCurrentDirectoryW(locations->GetBasePath().c_str());
	debug->Log(L"current directory is set to %s", locations->GetBasePath().c_str());
	std::string cp = resources->GetClassPath();
	if (cp.empty())
	{
		exceptionWrapper->ThrowException(L"classpath is not defined", resources->GetClassPathIsNotDefinedMessage());
	}
	debug->Log("class path is %s", cp.c_str());
	std::string vmOptions = "";
	for (int n = 0; n < vmOptionLines.size(); n++)
	{
		vmOptions = vmOptions + " " + vmOptionLines[n];
	}
	debug->Log("vmoptions is %s", vmOptions.c_str());
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
	std::wstring cmdLine = format_message(L"%s\\javaw.exe -cp %s %s %s", binDir.c_str(), to_wstring_(cp).c_str(), to_wstring_(vmOptions).c_str(), to_wstring_(resources->GetMainClass()).c_str());
	debug->Log(L"command line is %s", cmdLine.c_str());
	if (CreateProcessW(NULL, &cmdLine[0], NULL, NULL,
		TRUE, 1000, NULL, locations->GetBasePath().c_str(), &si, &pi))
	{
		hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", L"",
			WS_POPUP | SS_BITMAP,
			0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, *hInstance, NULL);
		debug->Log(L"process was created");
		if (!SetTimer(hWnd, ID_TIMER, 1000 /* 1s */, TimerProc))
		{
			debug->Log(L"unable to create timer");
			dwExitCode = OTHER_ERROR_CODE;
			*needRestart = false;
			return;
		}
	}
	else
	{
		debug->Log(L"unable to create process");
		dwExitCode = OTHER_ERROR_CODE;
		*needRestart = false;
		return;
	}
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	debug->Log(L"exit code is %d", dwExitCode);
	if (dwExitCode == resources->GetRestartExitCode()) {
		*needRestart = true;
	}
}
std::wstring JVM::findJavaHome() {
	if (!resources->IsUseInstalledJava()) {
		return locations->GetBasePath() + L"\\" + resources->GetEmbeddedJavaHomePath();
	}
	exceptionWrapper->ThrowException(L"JAVA_HOME is not defined", resources->GetJavaHomeIsNotDefinedMessage());
}

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


