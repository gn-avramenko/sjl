#include "SingleInstanceChecker.h"
#include "Windows.h"

SingleInstanceChecker::SingleInstanceChecker(Resources* res, ExceptionWrapper* ew, Debug* aDebug) {
	resources = res;
	exceptionWrapper = ew;
	debug = aDebug;
	mutex = NULL;
}


BOOL SingleInstanceChecker::Check() {
	std::wstring mutexName = resources->GetMutexName();
	if(mutexName.empty()) {
		debug->Log("mutex is empty, can continue");
		return true;
	}
	std::wstring fullMutexName = L"Global\\"+mutexName;
	debug->Log("current handle is null");
	SetLastError(NULL);
	mutex = CreateMutexW(NULL, TRUE, fullMutexName.c_str());
	if (mutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
		debug->Log("handle can not be created");
		exceptionWrapper->ThrowException(L"instance of application is already running", resources->GetInstanceAlreadyRunningMessage());
		return FALSE;
	}
	debug->Log("handle was created");
	return TRUE;
}

void SingleInstanceChecker::MutexRelease()
{
	if (mutex) {
		ReleaseMutex(mutex);
		CloseHandle(mutex);
		mutex = NULL;
	}
}

