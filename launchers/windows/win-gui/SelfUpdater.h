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
public:
	SelfUpdater(Locations* aLocations, Resources *res, ExceptionWrapper* ew,  Debug* aDebug);
	BOOL IsUpdateRequired();
	void PerformUpdate();
};

