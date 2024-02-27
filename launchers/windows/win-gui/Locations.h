#pragma once
#include <string>
#include "Resources.h"
#include "ExceptionWrapper.h"
#include <Windows.h>

class Locations
{
private:
	std::wstring executablePath;
	std::wstring basePath;
	std::wstring jvmPath;
	std::wstring logFile;
	std::wstring sjlPath;
	std::wstring updateFile;
	std::wstring updateDirectory;
	std::wstring selfUpdateFile;
	std::wstring splashScreenFile;
	std::wstring optionsFile;
	Resources *resources;
	ExceptionWrapper* exceptionWrapper;
public:
	Locations(Resources *resources,ExceptionWrapper *ew);
	std::wstring GetLogFile();
	std::wstring GetSjlPath();
	std::wstring GetUpdateFile();
	std::wstring GetSelfUpdateFile();
	std::wstring GetUpdateDirectory();
	std::wstring GetSplashScreenFile();
	std::wstring GetOptionsFile();
	bool FileExists(std::wstring path);
	bool DirectoryExists(std::wstring path);
	void EnsureDirectoryExists(std::wstring path);
	void CopyDirectory(std::wstring source, std::wstring target);
	void FileCopy(std::wstring source, std::wstring target);
	void FileDelete(std::wstring source);
	void DirectoryRemove(std::wstring dir);
};

