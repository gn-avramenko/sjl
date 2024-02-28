#pragma once
#include <string>
#include "Locations.h"
#include "Resources.h"
#include "Debug.h"
#include "Windows.h"

#define SPLASH_WINDOW_CLASS_NAME L"SjlSplashWindow"

class SplashScreen
{
public:
	SplashScreen(HINSTANCE hInst, ExceptionWrapper* ew, Resources* res, Locations* locs, Debug* deb);
	void ShowSplash(std::wstring image);
	void HideSplash();
};

