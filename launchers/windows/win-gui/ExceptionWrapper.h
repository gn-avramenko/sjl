#pragma once
#include <string>
class ExceptionWrapper
{
private:
	std::wstring message;
	std::wstring developerMessage;

public:
	void ThrowException(std::wstring developerMessage, std::wstring message);
	void SetUserMessage(std::wstring usrMsg);
	void SetDeveloperMessage(std::wstring devMsg);
	std::wstring GetUserMessage();
	std::wstring GetDeveloperMessage();
};

