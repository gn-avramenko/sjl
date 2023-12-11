#ifndef WINGUI_H_
#define WINGUI_H_
#define ID_TIMER 1
#include <windows.h>

HWND getInstanceWindow();

VOID CALLBACK TimerProc(
  HWND hwnd,     // handle of window for timer messages
  UINT uMsg,     // WM_TIMER message
  UINT idEvent,  // timer identifier
  DWORD dwTime   // current system time
);


#endif /* WINGUI_H_ */
