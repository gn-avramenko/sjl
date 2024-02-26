#include <Windows.h>
#include "Resources.h"
#include "Debug.h"
#include "Locations.h"
#include "ExceptionWrapper.h"
#include "SingleInstanceChecker.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR pCmdLine, int nCmdShow) {	
	ExceptionWrapper exception;
	Resources resources(hInstance);
	Locations locations(resources, exception);
	Debug debug(pCmdLine, locations);
	debug.DumpLocations(locations);
	debug.DumpResources(resources);
	SingleInstanceChecker sic(resources.GetMutexName(), resources.GetAppTitle(), debug);
	if (!sic.Check()) {
		return resources.GetInstanceAlreadyRunningExitCode();
	}
	return 0;
}
