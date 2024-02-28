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

HWND mainWindowHandle;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmID = LOWORD(wParam);
		switch (wmID)
		{

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void MyRegisterClass(HINSTANCE hInstance)
{
	COLORREF transparentColor = RGB(0, 0, 0);
	HBRUSH hbrTransparent = CreateSolidBrush(transparentColor);

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SJL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = hbrTransparent;
	//wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SJL);
	wcex.lpszMenuName = L"SJLLauncher";
	wcex.lpszClassName = L"SjlParentWindow";
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int cmd)
{
	HWND hwnd = CreateWindowExW(WS_EX_LAYERED, L"SjlParentWindow", L"Sjl", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 300, 100, nullptr, nullptr, hInstance, nullptr);
	mainWindowHandle = hwnd;
	if (!hwnd)
		return 0;
	SetWindowLong(hwnd, GWL_STYLE, 0);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);	
	return 0;
}

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
		MyRegisterClass(hInstance);
		InitInstance(hInstance, nCmdShow);
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
