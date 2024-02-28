#include "Splash.h"
#include <string>

using namespace std;

DWORD h1dwExitCode = 0;
bool h1debug_flag = false;
HINSTANCE h1hInst;
std::wstring base_path;
wstring executable_path;
wstring program_params;
wstring jvm_path;
wstring exception_message;
wstring sjl_path;
HANDLE mutex = NULL;
HANDLE log_file;
HANDLE splash_screen_thread;
HBITMAP h_splash_bitmap, h_splash_old_bitmap;
HDC h_dc, h_dc_mem;
BITMAP splash_bm;
PAINTSTRUCT splash_ps;
RECT splash_rect;
HWND splash_window_handle;
DWORD process_id;

#define SPLASH_WINDOW_CLASS_NAME L"SplashWindow"

LRESULT CALLBACK splash_window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DWORD WINAPI show_splash_function(LPVOID lpParam)
{
	WNDCLASSW wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = splash_window_procedure;
	wc.hInstance = h1hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = SPLASH_WINDOW_CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	RegisterClassW(&wc);

	int x = (GetSystemMetrics(SM_CXSCREEN) - splash_bm.bmWidth) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - splash_bm.bmHeight) / 2;

	splash_window_handle = CreateWindowW(wc.lpszClassName, L"",
		(WS_BORDER),
		x, y, splash_bm.bmWidth, splash_bm.bmHeight, NULL, NULL, h1hInst, NULL);
	SetWindowLong(splash_window_handle, GWL_STYLE, 0); // remove all window styles, check MSDN for details

	ShowWindow(splash_window_handle, SW_SHOW);
	UpdateWindow(splash_window_handle);
	MSG msg;
	while (GetMessage(&msg, splash_window_handle, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_CLOSE)
		{
			break;
		}
	}
	return 0;
}
void show_splash(wstring image_path)
{
	//ManualSynchronization on 10.01.24 13:48
	h_splash_bitmap = (HBITMAP)LoadImageW(h1hInst, image_path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	/*	h_splash_bitmap = (HBITMAP)LoadImageA(hInst, "sample_24bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (!h_splash_bitmap)
		{
			h_splash_bitmap = (HBITMAP)LoadImageA(hInst, "sample_24bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		*///ManualSynchronization ends
	if (!h_splash_bitmap)
	{
		throw std::exception();
	}
	GetObject(h_splash_bitmap, sizeof(BITMAP), &splash_bm);
	DWORD threadId;
	splash_screen_thread = CreateThread(
		NULL,
		0,
		show_splash_function,
		NULL,
		0,
		&threadId);
}

void hide_current_splash()
{
	PostMessage(splash_window_handle, WM_CLOSE, 0, 0);
	WaitForSingleObject(splash_screen_thread, INFINITE);
	UnregisterClassW(SPLASH_WINDOW_CLASS_NAME, h1hInst);
}

LRESULT CALLBACK splash_window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		h_dc = GetDC(hWnd);
		h_dc_mem = CreateCompatibleDC(h_dc);
		h_splash_old_bitmap = (HBITMAP)SelectObject(h_dc_mem, h_splash_bitmap);
		ReleaseDC(hWnd, h_dc);
		return 0;
	case WM_PAINT:
		h_dc = BeginPaint(hWnd, &splash_ps);

		GetClientRect(hWnd, &splash_rect);
		SetStretchBltMode(h_dc, STRETCH_HALFTONE);
		StretchBlt(h_dc, 0, 0, splash_rect.right, splash_rect.bottom,
			h_dc_mem, 0, 0, splash_bm.bmWidth, splash_bm.bmHeight, SRCCOPY);

		EndPaint(hWnd, &splash_ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		DeleteDC(h_dc_mem);
		DeleteObject(h_splash_bitmap);
		DeleteObject(h_splash_old_bitmap);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Splash::Splash(HINSTANCE inst)
{
	h1hInst = inst;
}

void Splash::ShowSplash(std::wstring image)
{
	show_splash(image);
}

void Splash::HideSplash()
{
	hide_current_splash();
}
