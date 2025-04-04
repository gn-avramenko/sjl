#include "SplashScreen.h"
#include "Utils.h"

HINSTANCE instance;
ExceptionWrapper* exceptionWrapper;
Resources* resources;
BITMAP splashBm;
HDC h_dc, h_dc_mem;
HBITMAP hBitmap;
PAINTSTRUCT splash_ps;
RECT splash_rect;
HANDLE splash_screen_thread;
HBITMAP oldHBitmap;
Debug* debug;
bool splashVisible;
HWND splashWindowHandle;
Locations* locations;

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

void registerMainWindowClass(HINSTANCE hInstance, ExceptionWrapper* exceptionWrapper, Resources* resources)
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
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = hbrTransparent;
	wcex.lpszMenuName = L"SJLLauncher";
	wcex.lpszClassName = L"SjlParentWindow";
	RegisterClassExW(&wcex);
	HWND hwnd = CreateWindowExW(WS_EX_LAYERED, L"SjlParentWindow", L"Sjl", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 300, 100, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hwnd, SW_SHOW);
	//keybd_event(VK_MENU, 0, 0, 0);  // Нажать Alt
	//keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);  // Отпустить Alt
	//SetForegroundWindow(hwnd);  // Теперь можно
	//
	UpdateWindow(hwnd);
}



LRESULT CALLBACK splashWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
		SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) & ~WS_CAPTION);
		h_dc = GetDC(hWnd);
		h_dc_mem = CreateCompatibleDC(h_dc);
		oldHBitmap = (HBITMAP)SelectObject(h_dc_mem, hBitmap);
		ReleaseDC(hWnd, h_dc);
		return 0;
	case WM_PAINT:
		h_dc = BeginPaint(hWnd, &splash_ps);

		GetClientRect(hWnd, &splash_rect);
		SetStretchBltMode(h_dc, STRETCH_HALFTONE);
		StretchBlt(h_dc, 0, 0, splash_rect.right, splash_rect.bottom,
			h_dc_mem, 0, 0, splashBm.bmWidth, splashBm.bmHeight, SRCCOPY);

		EndPaint(hWnd, &splash_ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		DeleteDC(h_dc_mem);
		DeleteObject(hBitmap);
		DeleteObject(oldHBitmap);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

DWORD WINAPI showSplashFunction(LPVOID lpParam)
{
		WNDCLASSW wc;

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = splashWindowProcedure;
		wc.hInstance = instance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		wc.lpszClassName = SPLASH_WINDOW_CLASS_NAME;
		wc.lpszMenuName = NULL;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;

		RegisterClassW(&wc);

		int x = (GetSystemMetrics(SM_CXSCREEN) - splashBm.bmWidth) / 2;
		int y = (GetSystemMetrics(SM_CYSCREEN) - splashBm.bmHeight) / 2;

		splashWindowHandle = CreateWindowW(wc.lpszClassName, L"",
			(WS_BORDER),
			x, y, splashBm.bmWidth, splashBm.bmHeight, NULL, NULL, instance, NULL);

		ShowWindow(splashWindowHandle, SW_SHOW);
		UpdateWindow(splashWindowHandle);
		debug->Log("splash window was created");
		MSG msg;
		while (GetMessage(&msg, splashWindowHandle, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_CLOSE)
			{
				break;
			}
		}
		debug->Log("splash is hidden");
		return 0;	
}



SplashScreen::SplashScreen(HINSTANCE hInst, ExceptionWrapper* ew, Resources* res, Locations* locs, Debug* deb)
{
	locations = locs;
	exceptionWrapper = ew;
    debug = deb;
	resources = res;
	instance = hInst;	
}

void SplashScreen::ShowSplash(std::wstring image)
	
{	
	if (!locations->FileExists(image)) {
		exceptionWrapper->ThrowException(format_message(L"Splash file %s does not exist", image.c_str()), format_message(resources->GetUnableToLoadBitmapMessage(), image.c_str()));
		return;
	}
	hBitmap = (HBITMAP)LoadImageW(instance, image.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hBitmap)
	{
		exceptionWrapper->ThrowException(format_message(L"Unable to load bitmap from %s, probably bad format", image.c_str()), format_message(resources->GetUnableToLoadBitmapMessage(), image.c_str()));
		return;
	}
	registerMainWindowClass(instance, exceptionWrapper, resources);
	GetObject(hBitmap, sizeof(BITMAP), &splashBm);
	DWORD threadId;
	splash_screen_thread = CreateThread(
		NULL,
		0,
		showSplashFunction,
		NULL,
		0,
		&threadId);
	splashVisible = true;
	debug->Log("splash thread was created");
	Sleep(100);

}

BOOLEAN SplashScreen::isSplashVisibe() {
	return splashVisible;
}

void SplashScreen::HideSplash()
{
	debug->Log("hiding splash");
	if (!splashVisible) {
		debug->Log("splash is not visible");
		return;
	}
	PostMessage(splashWindowHandle, WM_CLOSE, 0, 0);
	WaitForSingleObject(splash_screen_thread, INFINITE);
	UnregisterClassW(SPLASH_WINDOW_CLASS_NAME, instance);
	splashVisible = false;
	debug->Log("splash screen is hidden");
}
