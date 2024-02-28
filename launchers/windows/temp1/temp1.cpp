#include "Windows.h"
#include "Splash.h"



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR pCmdLine, int nCmdShow)
{
	Splash splash(hInstance);
	splash.ShowSplash(L"c:\\IdeaProjects\\temp4\\sjl\\sample.bmp");
	Sleep(5000);
	splash.HideSplash();
	return 0;
}