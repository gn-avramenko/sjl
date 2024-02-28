#pragma once
#include <string>
#include <windows.h>

#define MAX_RESOURCE_VALUE_LENGTH 5000

class Resources
{
private:
	std::wstring sjlPath;
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
	std::wstring unableToLocateJvmDllMessage;
	std::wstring unableToLoadJvmDllMessage;
	std::wstring unableToFindAdressOfJNI_CreateJavaVMMessage;
	std::wstring errorMessage;
	std::wstring unableToLoadBitmapMessage;
	std::wstring splashScreenFileName;
	std::wstring vmOptionsFile;
	std::wstring embeddedJavaHomePath;
	std::string vmOptions;
	std::string classPath;
	std::wstring classPathIsNotDefinedMessage;
	int restartExitCode;
	std::wstring unableToCreateJVMMessage;
	std::wstring unableToFindMainClassMessage;
	std::wstring unableToFindMainMethodMessage;
	std::wstring errorInvokingMainMethodMessage;
	std::string mainClass;


public:
	Resources(HINSTANCE hInst);

	std::wstring GetSjlPath();

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
	std::wstring GetVMOptionsFile();
	std::string GetVMOptions();
	std::wstring GetEmbeddedJavaHomePath();
	std::wstring GetUnableToLocateJvmDllMessage();
	std::string GetClassPath();
	std::wstring GetClassPathIsNotDefinedMessage();
	std::wstring GetUnableToLoadJvmDllMessage();
	std::wstring GetUnableToFindAdressOfJNI_CreateJavaVMMessage();
	int GetRestartExitCode();
	std::wstring GetUnableToCreateJVMMessage();
	std::wstring GetUnableToFindMainClassMessage();
	std::wstring GetUnableToFindMainMethodMessage();
	std::wstring GetErrorInvokingMainMethodMessage();
	std::string GetMainClass();
};


