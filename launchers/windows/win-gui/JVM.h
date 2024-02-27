#pragma once
#include "Locations.h"
#include "Debug.h"
#include "Resources.h"
class JVM
{
private:
	ExceptionWrapper* exceptionWrapper;
	Locations* locations;
	Debug* debug;
	Resources* resources;
public:
	void LaunchJVM();
};

