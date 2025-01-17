#include <Windows.h>
#include <exception>
#include <stdexcept>
#include "Resources.h"
#include "Debug.h"
#include "Locations.h"
#include "ExceptionWrapper.h"
#include "AppUpdater.h"
#include "SelfUpdater.h"
#include "SingleInstanceChecker.h"
#include "JVMJni.h"
#include "JVM.h"
#include "SplashScreen.h"
#include "Utils.h"

std::wstring getCause(std::exception_ptr eptr);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow) {
	ExceptionWrapper exception;
	Debug* d = nullptr;
	Resources* r = nullptr;
	std::wstring commandLine = std::wstring(pCmdLine);
	commandLine = replace(commandLine, L"-sjl-restart", L"");
	/*
	if (commandLine.find(L"-sjl-restart") != std::wstring::npos) {
		commandLine.replace(str.find(str2), str2.length(), str3L"-sjl-restart", L"")
	}
	*/
	SingleInstanceChecker* c = nullptr;
	try {
		bool needRestart = false;
		Resources resources(hInstance);
		r = &resources;
		Locations locations(&resources, &exception);
		Debug debug(pCmdLine, &locations, &resources, &exception);
		d = &debug;
		debug.DumpLocations();
		debug.DumpResources();
		SingleInstanceChecker sic(&resources, &exception, &debug);
		if (!sic.Check()) {
			sic.MutexRelease();
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
		IJVM* ijvm = nullptr;
		if (resources.IsUseJni()) {
			JVMJni jvm(&exception, &locations, &debug, &resources, &sic, &splashScreen, pCmdLine);
			ijvm = &jvm;
		}
		else {
			JVM jvm(&hInstance, &exception, &locations, &debug, &resources, &sic, &splashScreen, &commandLine, &needRestart);
			ijvm = &jvm;
		}
		ijvm->LaunchJVM();
		debug.Log(L"need restart is %s", needRestart);
		sic.MutexRelease();
		debug.CloseHandle();
		if (needRestart) {
			std::wstring params = pCmdLine;
			std::wstring newParams = params.find(L"-sjl-restart") == std::wstring::npos ? params + L" -sjl-restart" : params;
			ShellExecuteW(NULL, L"open", locations.GetExecutablePath().c_str(), newParams.c_str(), NULL, SW_RESTORE);
		}
		return 0;
	}
	catch (...) {
		std::exception_ptr eptr = std::current_exception();
		std::wstring cause = getCause(eptr);
		if (exception.GetDeveloperMessage().empty()) {
			exception.SetDeveloperMessage(cause);
		}
		if (exception.GetUserMessage().empty()) {
			exception.SetUserMessage(cause);
		}
		if (c != nullptr) {
			c->MutexRelease();
		}
		if (d != nullptr) {
			d->Log(exception.GetDeveloperMessage());
			d->CloseHandle();
		}
		MessageBoxW(0, exception.GetUserMessage().c_str(), r == nullptr ? L"Error" : r->GetErrorTitle().c_str(), 0);
		return 1;
	}
}

std::wstring getCause(std::exception_ptr eptr) {
	try
	{
		if (eptr)
			std::rethrow_exception(eptr);
	}
	catch (const std::exception& ex)
	{
		const char* cause = ex.what();
		return std::wstring(cause, cause + strlen(cause));
	}
	return L"Can't get exception cause, probably std::bad_exception (managed).";
}
