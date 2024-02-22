#pragma once
#include <string>
#include "Resources.h"
#include "Debug.h"
#include "ExceptionWrapper.h"
#include <Windows.h>

class Locations
{
private:
	std::wstring executable_path;
	std::wstring base_path;
	std::wstring jvm_path;
	std::wstring log_file;
	std::wstring sjl_path;
	ExceptionWrapper exception_wrapper;
public:
	Locations(Resources resources, ExceptionWrapper ew);
	void DumpLocations(Debug debug);
	std::wstring GetLogFile();
	std::wstring GetSjlPath();
	boolean IsFileExists(std::wstring path);
	boolean IsDirectoryExists(std::wstring path);
	void EnsureDirectoryExists(std::wstring path);
};

