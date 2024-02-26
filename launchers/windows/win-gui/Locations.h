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
	ExceptionWrapper exceptionWrapper;
public:
	Locations(Resources resources, ExceptionWrapper ew);
	std::wstring GetLogFile();
	std::wstring GetSjlPath();
	boolean IsFileExists(std::wstring path);
	boolean IsDirectoryExists(std::wstring path);
	void EnsureDirectoryExists(std::wstring path);
};

