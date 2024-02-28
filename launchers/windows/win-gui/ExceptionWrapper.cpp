#include "ExceptionWrapper.h"

void ExceptionWrapper::ThrowException(std::wstring devMsg, std::wstring userMsg) {
	message = userMsg;
	developerMessage = devMsg;
	throw std::exception();
}

std::wstring ExceptionWrapper::GetUserMessage() {
	return message;
}

std::wstring ExceptionWrapper::GetDeveloperMessage() {
	return developerMessage;
}