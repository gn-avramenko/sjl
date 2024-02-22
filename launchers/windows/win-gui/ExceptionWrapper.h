#pragma once
#include <string>
class ExceptionWrapper
{
private:
	std::wstring message;

public:
	void ThrowException(std::wstring message);
	std::wstring GetMessage();
};

