#pragma once
#include "Locations.h"
#include "Resources.h"
#include "Debug.h"
#include "ExceptionWrapper.h"
#include "SplashScreen.h"

class AppUpdater
{
private:
	Locations *locations;
	Debug *debug;
	ExceptionWrapper *exceptionWrapper;
	Resources* resources;
	SplashScreen* splashScreen;
public:
	AppUpdater(Locations *aLocations, ExceptionWrapper *ew, Resources *res, SplashScreen *splash, Debug *aDebug);
	BOOL IsUpdateRequired();
	void PerformUpdate();
};

