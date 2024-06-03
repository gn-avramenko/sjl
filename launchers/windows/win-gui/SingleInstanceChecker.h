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
	Resources* resources;
	ExceptionWrapper* exceptionWrapper;
	Debug *debug;
	HANDLE mutex;
public:
	SingleInstanceChecker(Resources* res, ExceptionWrapper* ew, Debug *aDebug);
	BOOL Check();
	void MutexRelease();
};

