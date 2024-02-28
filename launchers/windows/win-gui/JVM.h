#pragma once
#include "Locations.h"
#include "Debug.h"
#include "Resources.h"
#include "SplashScreen.h"
#include "SingleInstanceChecker.h"
#include <vector>
#include <string>

class JVM
{
private:
	ExceptionWrapper* exceptionWrapper;
	Locations* locations;
	Debug* debug;
	Resources* resources;
	void add_option(std::string token, std::vector<std::string>& vmOptionLines);
	std::wstring findJavaHome();
	SplashScreen* splashScreen;
public:
	JVM(ExceptionWrapper* ew, Locations* loc, Debug* deb, Resources* res, SingleInstanceChecker* sic, SplashScreen* splash, std::string programParams);
	void LaunchJVM();
};

