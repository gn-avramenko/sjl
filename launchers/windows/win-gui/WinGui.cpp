#include <Windows.h>
#include "Resources.h"
#include "Debug.h"
#include "Locations.h"
#include "ExceptionWrapper.h"
#include "AppUpdater.h"
#include "SelfUpdater.h"
#include "SingleInstanceChecker.h"
#include "JVM.h"
#include "SplashScreen.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR pCmdLine, int nCmdShow) {	
	ExceptionWrapper exception;
	Debug* d = nullptr;
	Resources* r = nullptr;
	try {
		Resources resources(hInstance);
		r = &resources;
		Locations locations(&resources, &exception);		
		Debug debug(pCmdLine, &locations, &resources, &exception);
		d = &debug;
		debug.DumpLocations();
		debug.DumpResources();
		SingleInstanceChecker sic(resources.GetMutexName(), resources.GetAppTitle(), &debug);
		if (!sic.Check()) {
			return resources.GetInstanceAlreadyRunningExitCode();
		}
		SplashScreen splashScreen(hInstance, &exception, &resources, &locations, &debug);		
		AppUpdater appUpdater(&locations, &exception, &resources, nullptr, &debug);
		bool deleteUpdateDirectory = false;
		if (appUpdater.IsUpdateRequired()) {
			appUpdater.PerformUpdate();
			deleteUpdateDirectory = true;
		}
		SelfUpdater selfUpdater(&locations, &resources, &exception, &debug);
		if (selfUpdater.IsUpdateRequired()) {
			selfUpdater.PerformUpdate();
		}
		if (deleteUpdateDirectory) {
			locations.DirectoryRemove(locations.GetUpdateDirectory());
		}
		if (!locations.GetSplashScreenFile().empty()) {
			splashScreen.ShowSplash(locations.GetSplashScreenFile());			
		}
		JVM jvm(&exception, &locations, &debug, &resources, &sic, &splashScreen, pCmdLine);
		jvm.LaunchJVM();
		debug.CloseHandle();
		return 0;
	}
	catch (...) {
		if (d != nullptr) {
			d->Log(exception.GetDeveloperMessage());
			d->CloseHandle();
		}
		MessageBoxW(0, exception.GetUserMessage().c_str(), r == nullptr? L"Error": r->GetErrorMessage().c_str(), 0);
		return 1;
	}
}
