#include "ExceptionWrapper.h"

void ExceptionWrapper::ThrowException(std::wstring developerMessage, std::wstring msg) {
	message = msg;
	developerMessage = msg;
	throw std::exception();
}

std::wstring ExceptionWrapper::GetUserMessage() {
	return message;
}

std::wstring ExceptionWrapper::GetDeveloperMessage() {
	return developerMessage;
}