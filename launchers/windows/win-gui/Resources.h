#pragma once
#include <string>
#include <windows.h>

#define MAX_RESOURCE_VALUE_LENGTH 5000

class Resources
{
private:
	std::wstring sjl_path;
	std::wstring jvm_path;

public:
	Resources(HINSTANCE hInst);

	std::wstring GetSjlPath();
	
	std::wstring GetJvmPath();
};

