#pragma once
#include <Windows.h>
#include "Locations.h"

class Debug
{
private:
    bool debugFlag;
    FILE* handle;
public:
    Debug(LPSTR pCmdLine, Locations locations);
    ~Debug();
    void Log(std::wstring format, ...);
    void Log(std::string format, ...);
    void DumpLocations(Locations locations);
    void DumpResources(Resources resources);

};

