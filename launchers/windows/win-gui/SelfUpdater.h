#pragma once
#include "Locations.h"
#include "Debug.h"

class SelfUpdater
{
private:
	Locations* locations;
	Debug* debug;	
	ExceptionWrapper* exceptionWrapper;
	Resources* resources;
	PWSTR programParams;
	std::wstring getNewLauncherFileName(BOOL updateStarted);
	std::wstring getCurrentLauncherFileName();
public:
	SelfUpdater(Locations* aLocations, Resources *res, PWSTR programParams, ExceptionWrapper* ew,  Debug* aDebug);
	BOOL IsUpdateRequired();
	void PerformUpdate();
};

