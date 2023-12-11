#ifndef WIN_COMMON_H
#define WIN_COMMON_H
//https://unix.stackexchange.com/questions/604260/best-range-for-custom-exit-code-in-linux
#define RESTART_EXIT_CODE 79
#define ID_TIMER 1
#include <string>
#include <windows.h>
#include <jni.h>
#include <jni_md.h>
typedef void (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
void debug(std::wstring message);
bool prepare(LPCSTR lpCmdLine);
void close_log_file_handle();
void throw_exception(std::wstring message);
std::wstring get_error_message();
std::wstring get_jvm_path();
#endif //WIN_COMMON_H
