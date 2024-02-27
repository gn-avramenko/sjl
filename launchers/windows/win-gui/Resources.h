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
	std::wstring unableToOpenFileMessage;
	std::wstring unableToCreateDirectoryMessage;
	std::wstring sourceFileDoesNotExistMessage;
	std::wstring unableToCopyDirectoryMessage;
	std::wstring unableToCopyFileMessage;
	std::wstring unableToDeleteDirectoryMessage;
	std::wstring unableToDeleteFileMessage;
	std::wstring unableToRenameFileMessage;
	std::wstring unableToPerformSelfUpdateMessage;
	std::wstring errorMessage;
	std::wstring unableToLoadBitmapMessage;
	std::wstring splashScreenFileName;
	std::wstring optionsFile;

public:
	Resources(HINSTANCE hInst);

	std::wstring GetSjlPath();
	
	std::wstring GetJvmPath();

	std::wstring GetMutexName();

	int GetInstanceAlreadyRunningExitCode() const;

	std::wstring GetAppTitle();

	std::wstring GetUnableToOpenFileMessage();

	std::wstring GetUnableToCreateDirectoryMessage();

	std::wstring GetSourceFileDoesNotExistMessage();
	std::wstring GetUnableToCopyDirectoryMessage();
	std::wstring GetUnableToCopyFileMessage();
	std::wstring GetUnableToDeleteDirectoryMessage();
	std::wstring GetUnableToDeleteFileMessage();
	std::wstring GetUnableToRenameFileMessage();
	std::wstring GetUnableToPerformSelfUpdateMessage();
	std::wstring GetErrorMessage();
	std::wstring GetUnableToLoadBitmapMessage();
	std::wstring GetSplashScreenFile();
	std::wstring GetOptionsFile();
};


