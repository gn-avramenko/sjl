#pragma once
#include "Locations.h"
#include "Debug.h"
#include "Resources.h"
#include "SplashScreen.h"
#include "SingleInstanceChecker.h"
#include <vector>
#include <string>
#include "ijvm.h"

class JVM: public IJVM
{
private:
	HINSTANCE* hInstance;
	ExceptionWrapper* exceptionWrapper;
	Locations* locations;
	Debug* debug;
	Resources* resources;
	bool* needRestart;
	SingleInstanceChecker* sic;
	std::string programParams;
	SplashScreen* splashScreen;
	void add_option(std::string token, std::vector<std::string>& vmOptionLines);
	std::wstring findJavaHome();
public:
	JVM(HINSTANCE* inst, ExceptionWrapper* ew, Locations* loc, Debug* deb, Resources* res, SingleInstanceChecker* sic, SplashScreen* splash, std::wstring programParams, bool* needRestart);
	void LaunchJVM();
};

