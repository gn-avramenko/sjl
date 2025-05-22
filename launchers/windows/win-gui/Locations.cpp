#include "Locations.h"
#include "ExceptionWrapper.h"
#include "Utils.h"
#include "Windows.h"

Locations::Locations(Resources *res, ExceptionWrapper* ew) {
	exceptionWrapper = ew;
	resources = res;
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	std::wstring pathStr = path;
	executablePath = path;
	size_t position = pathStr.find_last_of(L"\\");
	basePath = pathStr.substr(0, position);
	jvmPath = basePath + L"\\" + resources->GetEmbeddedJavaHomePath();
	sjlPath = basePath + L"\\" + resources->GetSjlPath();
	logFile = sjlPath + L"\\sjl";
	updateDirectory = sjlPath + L"\\update";
	updateFile = updateDirectory + L"\\update.script";
	selfUpdateFile = updateDirectory + L"\\self-update.script";
	splashScreenFile = resources->GetSplashScreenFile().empty()?std::wstring(): basePath + L"\\" + resources->GetSplashScreenFile();
	optionsFile = resources->GetVMOptionsFile().empty() ? std::wstring() : basePath + L"\\" + resources->GetVMOptionsFile();
}

std::wstring Locations::GetLogFile() {
	return logFile;
}

std::wstring Locations::GetSjlPath() {
	return sjlPath;
}

std::wstring Locations::GetUpdateFile()
{
	return updateFile;
}

std::wstring Locations::GetSelfUpdateFile()
{
	return selfUpdateFile;
}

std::wstring Locations::GetUpdateDirectory()
{
	return updateDirectory;
}

std::wstring Locations::GetSplashScreenFile()
{
	return splashScreenFile;
}

std::wstring Locations::GetOptionsFile()
{
	return optionsFile;
}

std::wstring Locations::GetBasePath()
{
	return basePath;
}

std::wstring Locations::GetExecutablePath()
{
	return executablePath;
}

void Locations::EnsureDirectoryExists(std::wstring path) {
	if (DirectoryExists(path)) {
		return;
	}
	if (!CreateDirectoryW(path.c_str(), NULL)) {
		exceptionWrapper->ThrowException(format_message(L"Unable to create direcotory %s.", path.c_str()), format_message(resources->GetUnableToCreateDirectoryMessage(), path.c_str()));
	}
}

bool Locations::FileExists(std::wstring path) {
	LPCWSTR szPath = path.c_str();
	DWORD dwAttrib = GetFileAttributesW(szPath);
	return dwAttrib != INVALID_FILE_ATTRIBUTES;
}

bool Locations::DirectoryExists(std::wstring path) {
	if (!FileExists(path)) {
		return false;
	}
	DWORD attrib = GetFileAttributesW(path.c_str());
	return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool CreateDirectoriesRecursive(const std::wstring& path) {
	size_t pos = 0;
	std::wstring dir;

	// Обработка UNC-путей (\\server\share\...)
	if (path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\') {
		pos = path.find(L'\\', 2);
		if (pos != std::wstring::npos) {
			pos = path.find(L'\\', pos + 1);
		}
	}

	while (pos != std::wstring::npos) {
		dir = path.substr(0, pos);
		if (!dir.empty() && dir.back() != L':') { // Пропускаем "C:" (диск)
			if (!CreateDirectoryW(dir.c_str(), NULL)) {
				DWORD err = GetLastError();
				if (err != ERROR_ALREADY_EXISTS) {
					return false;
				}
			}
		}
		pos = path.find(L'\\', pos + 1);
	}
	return true;
}


void Locations::FileCopy(std::wstring source, std::wstring target) {
	size_t lastSlash = target.find_last_of(L'\\');
	if (lastSlash != std::wstring::npos) {
		std::wstring destDir = target.substr(0, lastSlash+1);
		if (!CreateDirectoriesRecursive(destDir)) {
			exceptionWrapper->ThrowException(format_message(L"Error: Failed to copy file from %s to %s, details: unable to create directory subtree", source.c_str(), target.c_str()),
				format_message(resources->GetUnableToCopyFileMessage(), source.c_str(), target.c_str()));
		}
	}

	if(CopyFileW(source.c_str(), target.c_str(), FALSE) == 0) {
		DWORD error = GetLastError();
		exceptionWrapper->ThrowException(format_message(L"Error: %d. Failed to copy file from %s to %s.", error, source.c_str(), target.c_str()),
			format_message(resources->GetUnableToCopyFileMessage(), source.c_str(), target.c_str()));
	}
}

void Locations::FileDelete(std::wstring source)
{
	if (DeleteFileW(source.c_str()) == 0) {
		DWORD error = GetLastError();
		exceptionWrapper->ThrowException(format_message(L"Error: %d. Failed tot delete file %s.", error, source.c_str()),
			format_message(resources->GetUnableToDeleteFileMessage(), source.c_str()));
	}
}

void Locations::DirectoryRemove(std::wstring dir)
{
	dir.push_back(L'\0');
	SHFILEOPSTRUCTW file_op = {
		NULL,
		FO_DELETE,
		dir.c_str(),
		L"",
		FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
		false,
		0,
		L"" };
	int result = SHFileOperationW(&file_op);
	if (result)
	{
		exceptionWrapper->ThrowException(format_message(L"unable to delete directory %s. Code=%s", dir.c_str(), std::to_wstring(result).c_str()),
			format_message(resources->GetUnableToDeleteDirectoryMessage(), dir.c_str(), std::to_wstring(result).c_str()));
	}
}

void Locations::CopyDirectory(std::wstring source, std::wstring target)
{
	source.push_back(L'\0');
	target.push_back(L'\0');
	SHFILEOPSTRUCTW file_op = {
		NULL,
		FO_COPY,
		source.c_str(),
		target.c_str(),
		FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
		false,
		0,
		L"" };
	int res = SHFileOperationW(&file_op);
	if (res) {
		exceptionWrapper->ThrowException(format_message(L"Unable to copy directory from %s to %s. Code: %s", source.c_str(), 
			target.c_str(), std::to_string(res)), format_message(resources->GetUnableToCopyDirectoryMessage(), source.c_str(), target.c_str(), std::to_string(res)));
	}
}

std::wstring Locations::ReadFileContent(std::wstring fileName) {
	FILE* f = NULL;
	int fo_res = _wfopen_s(&f, fileName.c_str(), L"rt, ccs=UTF-8");
	if (fo_res) {
		exceptionWrapper->ThrowException(format_message(L"unable to open file %s, code %s", fileName.c_str()), format_message(resources->GetUnableToOpenFileMessage(), fileName.c_str()));
	}
	std::wstring result;
	if (!fo_res)
	{
		wchar_t buffer[4096];
		if (fgetws(buffer, sizeof(buffer), f))
		{
			trim_line(buffer);
			result = buffer;
		}
		fclose(f);
	}
	return result;
}

void Locations::WriteFileContent(std::wstring fileName, std::wstring content) {
	FILE* handle = new FILE();
	if (_wfopen_s(&handle, fileName.c_str(), L"w, ccs=UTF-8")) {
		exceptionWrapper->ThrowException(format_message(L"unable to write to file %s, code %s", fileName.c_str()), format_message(resources->GetUnableToOpenFileMessage(), fileName.c_str()));
	}
	fputws(content.c_str(), handle);
	fflush(handle);
	fclose(handle);
}