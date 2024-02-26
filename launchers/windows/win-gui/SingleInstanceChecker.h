#pragma once
#include <string>
#include "Windows.h"
#include "Debug.h"

struct EnumWindowsData {
	const wchar_t* targetTitle;
	HWND hwnd;
};

class SingleInstanceChecker
{
private:
	std::wstring mutexName;
	std::wstring appTitle;
	Debug debug;

public:
	SingleInstanceChecker(std::wstring aMutexName, std::wstring anAppTitle, Debug aDebug);
	BOOL Check();
};

