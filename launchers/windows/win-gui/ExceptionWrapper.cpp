#include "ExceptionWrapper.h"

void ExceptionWrapper::ThrowException(std::wstring msg) {
	message = msg;
	throw std::exception();
}
