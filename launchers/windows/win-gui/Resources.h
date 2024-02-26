#pragma once
#include <string>
#include <windows.h>

#define MAX_RESOURCE_VALUE_LENGTH 5000

class Resources
{
private:
	std::wstring sjlPath;
	std::wstring jvmPath;
	std::wstring mutexName;
	int instanceAlreadyRunningExitCode;
	std::wstring appTitle;


public:
	Resources(HINSTANCE hInst);

	std::wstring GetSjlPath();
	
	std::wstring GetJvmPath();

	std::wstring GetMutexName();

	int GetInstanceAlreadyRunningExitCode() const;

	std::wstring GetAppTitle();
};

