#include "ExceptionWrapper.h"

void ExceptionWrapper::ThrowException(std::wstring msg) {
	message = msg;
	throw std::exception();
}

std::wstring ExceptionWrapper::GetMessage() {
	return message;
}
