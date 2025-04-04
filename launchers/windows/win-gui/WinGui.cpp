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
	boolean restart = commandLine.find(L"-sjlr") != std::wstring::npos;
	commandLine = replace(commandLine, L"-sjlr", L"");
	commandLine = replace(commandLine, L"-sjlu1", L"");
	commandLine = replace(commandLine, L"-sjlu2", L"");
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
		SelfUpdater selfUpdater(&locations, &resources, pCmdLine, &exception, &debug);
		SingleInstanceChecker sic(&resources, &exception, &debug);
		bool selfUpdateRequired = selfUpdater.IsUpdateRequired();
		SplashScreen splashScreen(hInstance, &exception, &resources, &locations, &debug);
		if (selfUpdateRequired) {
			selfUpdater.PerformUpdate();
		}
		if (!sic.Check()) {
			sic.MutexRelease();
			return resources.GetInstanceAlreadyRunningExitCode();
		}
		if (!locations.GetSplashScreenFile().empty()) {
			splashScreen.ShowSplash(locations.GetSplashScreenFile());
		}
		AppUpdater appUpdater(&locations, &exception, &resources, nullptr, &debug);
		bool deleteUpdateDirectory = false;
		if (appUpdater.IsUpdateRequired()) {
			appUpdater.PerformUpdate();
			deleteUpdateDirectory = true;
		}
		if (selfUpdateRequired || deleteUpdateDirectory) {
			locations.DirectoryRemove(locations.GetUpdateDirectory());
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
		debug.Log(L"need restart is %s", needRestart? L"true": L"false");
		sic.MutexRelease();
		debug.CloseHandle();
		if (needRestart) {
			std::wstring params = pCmdLine;
			std::wstring newParams = params.find(L"-sjlr") == std::wstring::npos ? params + L" -sjlr" : params;
			ShellExecuteW(NULL, L"open", locations.GetExecutablePath().c_str(), newParams.c_str(), NULL, SW_SHOW);
			Sleep(500);  
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
