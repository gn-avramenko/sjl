#include "Utils.h"
#include "Windows.h"
#include <vector>

void trim_line(char* line)
{
	char* p = line + strlen(line) - 1;
	if (p >= line && *p == '\n')
	{
		*p-- = '\0';
	}
	while (p >= line && (*p == ' ' || *p == '\t'))
	{
		*p-- = '\0';
	}
}

void trim_line(wchar_t* line)
{
	wchar_t* p = line + wcslen(line) - 1;
	if (p >= line && *p == '\n')
	{
		*p-- = '\0';
	}
	while (p >= line && (*p == ' ' || *p == '\t'))
	{
		*p-- = '\0';
	}
}

std::wstring format_message(std::wstring format, ...)
{
	va_list args;
	va_start(args, format);

	wchar_t buf[4024] = { 0, };
	_vsnwprintf_s(buf, sizeof(buf), format.c_str(), args);
	va_end(args);
	return buf;
}

std::wstring to_wstring_(std::string s)
{
	std::wstring ws(s.size(), L' '); // Overestimate number of code points.
	size_t size;
	mbstowcs_s(&size, &ws[0], s.size(), s.c_str(), s.size());
	ws.resize(size);
	return ws;
}