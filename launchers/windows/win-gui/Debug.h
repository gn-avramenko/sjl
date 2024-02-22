#pragma once
#include <Windows.h>
#include "Locations.h"

class Debug
{
private:
    bool debug_flag;
    FILE* handle;
public:
    Debug(LPSTR pCmdLine, Locations locations);
    ~Debug();
    void Log(wchar_t* format, ...);
    void Log(char* format, ...);

};

