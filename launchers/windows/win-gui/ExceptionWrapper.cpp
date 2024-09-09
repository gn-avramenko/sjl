#include "ExceptionWrapper.h"

void ExceptionWrapper::ThrowException(std::wstring devMsg, std::wstring usrMsg) {
	message = usrMsg;
	developerMessage = devMsg;
	throw std::exception();
}

std::wstring ExceptionWrapper::GetUserMessage() {
	return message;
}

std::wstring ExceptionWrapper::GetDeveloperMessage() {
	return developerMessage;
}

void ExceptionWrapper::SetUserMessage(std::wstring usrMsg) {
	message = usrMsg;
}

void ExceptionWrapper::SetDeveloperMessage(std::wstring devMsg) {
	developerMessage = devMsg;
}