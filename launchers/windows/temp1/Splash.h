#pragma once
#include "Windows.h"
#include <string>
class Splash
{
public:
	Splash(HINSTANCE inst);
	void ShowSplash(std::wstring image);
	void HideSplash();
};

