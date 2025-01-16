#include "Debug.h"
#include "Utils.h"
#include <string>
#include <time.h>

Debug::Debug(PWSTR pCmdLine, Locations *locs, Resources *res, ExceptionWrapper *ew) {
	resources = res;
	locations = locs;
	std::wstring cmdLineStr = pCmdLine;
	debugFlag = (cmdLineStr.find(L"-sjl-debug") != std::wstring::npos || res->IsSjlDebug());
	handle = new FILE();
	if (debugFlag) {
		locs->EnsureDirectoryExists(locations->GetSjlPath());
		std::wstring logFile = locations->GetLogFile();
		debugFlag = false;
		//after restart old handle is not closed immideatly
		for (size_t i = 0; i < 5; i++)
		{
			if (!_wfopen_s(&handle, logFile.c_str(), L"a, ccs=UTF-8")) {
				debugFlag = true;
				break;
			}
			Sleep(100);
		}
		return;
	}	
}

void Debug::CloseHandle()
{
	if (debugFlag) {
		fclose(handle);
	}
}

void Debug::Log(std::wstring format, ...) {
	if (!debugFlag) {
		return;
	}
	va_list args;
	va_start(args, format);

	wchar_t buf[4024] = { 0, };
	_vsnwprintf_s(buf, sizeof(buf), format.c_str(), args);
	va_end(args);

	wchar_t temp_time_data[256] = { 0 };
	wchar_t	new_line_str[256] = L"\n";
	struct tm tm_ptr;
	time_t raw;
	time(&raw);
	localtime_s(&tm_ptr, &raw);
	wcsftime(temp_time_data, 256, L"'%Y-%m-%d %H:%M:%S' ", &tm_ptr);
	fputws(new_line_str, handle);
	fputws(temp_time_data, handle);
	fputws(buf, handle);
	fflush(handle);    
}

void Debug::Log(std::string format, ...) {
	if (!debugFlag) {
		return;
	}
	WCHAR buf[1024] = { 0 };
	va_list args;
	va_start(args, format);

	char buf2[2048] = { 0, };
	_vsnprintf_s(buf2, sizeof(buf2), format.c_str(), args);
	va_end(args);

	MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024);

	wchar_t temp_time_data[256] = { 0 };
	wchar_t	new_line_str[256] = L"\n";
	struct tm tm_ptr;
	time_t raw;
	time(&raw);
	localtime_s(&tm_ptr, &raw);

	wcsftime(temp_time_data, 256, L"'%Y-%m-%d %H:%M:%S' ", &tm_ptr);

	fputws(new_line_str, handle);
	fputws(temp_time_data, handle);
	fputws(buf, handle);
	fflush(handle);

}

void Debug::DumpLocations() {
	Log(L"SJL directory is %s", locations->GetSjlPath().c_str());
	Log(L"SJL log file is %s", locations->GetLogFile().c_str());
	Log(L"updateFile is %s", locations->GetUpdateFile().c_str());
	Log(L"selfUpdateFile is %s", locations->GetSelfUpdateFile().c_str());
	Log(L"updateDirectory is %s", locations->GetUpdateDirectory().c_str());
	Log(L"splashScreenFile is %s", locations->GetSplashScreenFile().c_str());
	Log(L"optionsFile is %s", locations->GetOptionsFile().c_str());
	Log(L"basePath is %s", locations->GetBasePath().c_str());
	Log(L"executablePath is %s", locations->GetExecutablePath().c_str());	
}

void Debug::DumpResources()
{
	Log("sjlDebug is %s", std::to_string(resources->IsSjlDebug()).c_str());
	Log(L"sjl path is %s", resources->GetSjlPath().c_str());
	Log(L"mutex name is %s", resources->GetMutexName().c_str());
	Log(L"instance already running exit code is %d", resources->GetInstanceAlreadyRunningExitCode());
	Log(L"unableToOpenFileMessage is %s", resources->GetUnableToOpenFileMessage().c_str());
	Log(L"unableToCreateDirectoryMessage is %s", resources->GetUnableToCreateDirectoryMessage().c_str());
	Log(L"sourceFileDoesNotExistMessage is %s", resources->GetSourceFileDoesNotExistMessage().c_str());
	Log(L"unableToCopyDirectoryMessage is %s", resources->GetUnableToCopyDirectoryMessage().c_str());
	Log(L"unableToCopyFileMessage is %s", resources->GetUnableToCopyFileMessage().c_str());
	Log(L"unableToDeleteDirectoryMessage is %s", resources->GetUnableToDeleteDirectoryMessage().c_str());
	Log(L"unableToDeleteFileMessage is %s", resources->GetUnableToDeleteFileMessage().c_str());
	Log(L"unableToRenameFileMessage is %s", resources->GetUnableToRenameFileMessage().c_str());
	Log(L"unableToPerformSelfUpdateMessage is %s", resources->GetUnableToPerformSelfUpdateMessage().c_str());
	Log(L"errorTitle is %s", resources->GetErrorTitle().c_str());
	Log(L"unableToLoadBitmapMessage is %s", resources->GetUnableToLoadBitmapMessage().c_str());
	Log(L"splashScreenFile is %s", resources->GetSplashScreenFile().c_str());
	Log(L"vmOptionsFile is %s", resources->GetVMOptionsFile().c_str());
	Log("vmOptions is %s", resources->GetVMOptions().c_str());
	Log(L"embeddedJavaHomePath is %s", resources->GetEmbeddedJavaHomePath().c_str());
	Log(L"unableToLocateJvmDllMessage is %s", resources->GetUnableToLocateJvmDllMessage().c_str());
	Log("classPath is %s", resources->GetClassPath().c_str());
	Log(L"classPathIsNotDefinedMessage is %s", resources->GetClassPathIsNotDefinedMessage().c_str());
	Log(L"unableToLoadJvmDllMessage is %s", resources->GetUnableToLoadJvmDllMessage().c_str());
	Log(L"unableToFindAdressOfJNI_CreateJavaVMMessage is %s", resources->GetUnableToFindAdressOfJNI_CreateJavaVMMessage().c_str());
	Log(L"restartExitCode is %d", resources->GetRestartExitCode());
	Log(L"unableToCreateJVMMessage is %s", resources->GetUnableToCreateJVMMessage().c_str());
	Log(L"unableToFindMainClassMessage is %s", resources->GetUnableToFindMainClassMessage().c_str());
	Log(L"unableToFindMainMethodMessage is %s", resources->GetUnableToFindMainMethodMessage().c_str());
	Log(L"errorInvokingMainMethodMessage is %s", resources->GetErrorInvokingMainMethodMessage().c_str());
	Log(L"instanceAlreadyRunningMessage is %s", resources->GetInstanceAlreadyRunningMessage().c_str());
	Log(L"javaHomeIsNotDefinedMessage is %s", resources->GetJavaHomeIsNotDefinedMessage().c_str());
	Log(L"wrongJavaTypeMessage is %s", resources->GetWrongJavaTypeMessage().c_str());
	Log(L"unableToCheckInstalledJavaMessage is %s", resources->GetUnableToCheckInstalledJavaMessage().c_str());
	Log("mainClass is %s", resources->GetMainClass().c_str());
	Log("useInstalledJava is %s", std::to_string(resources->IsUseInstalledJava()).c_str());
	Log("required64JRE is %s", std::to_string(resources->IsRequired64JRE()).c_str());
	Log("minJavaVersion is %s", std::to_string(resources->GetMinJavaVersion()).c_str());
	Log("maxJavaVersion is %s", std::to_string(resources->GetMaxJavaVersion()).c_str());
}
