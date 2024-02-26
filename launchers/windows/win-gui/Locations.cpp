#include "Locations.h"
#include "ExceptionWrapper.h"

Locations::Locations(Resources resources, ExceptionWrapper ew) {
	exception_wrapper = ew;
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	std::wstring pathStr = path;
	executable_path = path;
	size_t position = pathStr.find_last_of(L"\\");
	base_path = pathStr.substr(0, position);
	jvm_path = base_path + L"\\" + resources.GetJvmPath();
	sjl_path = base_path + L"\\" + resources.GetSjlPath();
	log_file = sjl_path + L"\\sjl.log";
}

std::wstring Locations::GetLogFile() {
	return log_file;
}

std::wstring Locations::GetSjlPath() {
	return sjl_path;
}

void Locations::EnsureDirectoryExists(std::wstring path) {
	if (IsDirectoryExists(path)) {
		return;
	}
	if (!CreateDirectoryW(sjl_path.c_str(), NULL))
	{
		exception_wrapper.ThrowException(L"unable to create directory " + sjl_path);
	}
}

boolean Locations::IsFileExists(std::wstring path) {
	LPCWSTR szPath = path.c_str();
	DWORD dwAttrib = GetFileAttributesW(szPath);
	return dwAttrib != INVALID_FILE_ATTRIBUTES;
}

boolean Locations::IsDirectoryExists(std::wstring path) {
	if (!IsFileExists(path)) {
		return false;
	}
	DWORD attrib = GetFileAttributesW(path.c_str());
	return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}