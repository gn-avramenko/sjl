#pragma once
#include <Windows.h>
#include "Locations.h"
#include "Resources.h"
#include "ExceptionWrapper.h"

class Debug
{
private:
    bool debugFlag;
    FILE* handle;
    Resources *resources;
    Locations *locations;
public:
    Debug(LPSTR pCmdLine, Locations* locations, Resources* res, ExceptionWrapper* ew);
    void CloseHandle();
    void Log(std::wstring format, ...);
    void Log(std::string format, ...);
    void DumpLocations();
    void DumpResources();

};

