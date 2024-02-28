#include "Debug.h"
#include "Utils.h"
#include <string>
#include <time.h>

Debug::Debug(LPSTR pCmdLine, Locations *locs, Resources *res, ExceptionWrapper *ew) {
	resources = res;
	locations = locs;
	std::string cmdLineStr = pCmdLine;
	debugFlag = (cmdLineStr.find("-sjl-debug") != std::string::npos);
	handle = new FILE();
	if (debugFlag) {
		locs->EnsureDirectoryExists(locations->GetSjlPath());
		std::wstring logFile = locations->GetLogFile();
		_wfopen_s(&handle, logFile.c_str(), L"a");	
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
}

void Debug::DumpResources()
{
	Log(L"APP_TITLE is %s", resources->GetAppTitle().c_str());

}
