#include "Debug.h"
#include <string>

Debug::Debug(LPSTR pCmdLine, Locations locations) {
	std::string cmdLineStr = pCmdLine;
	debug_flag = (cmdLineStr.find("-sjl-debug") != std::string::npos);
	if (debug_flag) {
		locations.EnsureDirectoryExists(locations.GetSjlPath());
		std::wstring log_file = locations.GetLogFile();
		handle = _wfopen(log_file.c_str(), L"a");		
	}
}

Debug::~Debug() {
	if (debug_flag) {
		fclose(handle);
	}
}
