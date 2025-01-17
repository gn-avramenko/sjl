#pragma once
#include <string>
void trim_line(char* line);
void trim_line(wchar_t* line);
std::wstring format_message(std::wstring format, ...);
std::wstring to_wstring_(std::string s);
std::wstring replace(std::wstring originalString, std::wstring fragment, std::wstring replacement);