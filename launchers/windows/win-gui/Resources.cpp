#include "Resources.h"


static std::wstring LoadStringFromResourceW(HINSTANCE hInstance, const wchar_t* ResourceName, std::wstring DefValue)
{
	HRSRC hResource = FindResourceExW(hInstance, ResourceName, RT_RCDATA, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	if (hResource == nullptr) {
		return DefValue;
	}

	HGLOBAL hGlobal = LoadResource(hInstance, hResource);
	if (hGlobal == nullptr) {
		return DefValue;
	}

	LPVOID lpResourceData = LockResource(hGlobal);
	if (lpResourceData == nullptr) {
		FreeResource(hGlobal);
		return DefValue;
	}

	int resourceSize = SizeofResource(hInstance, hResource);

	// Ensure the buffer is large enough
	if (resourceSize + 1 > MAX_RESOURCE_VALUE_LENGTH) {
		// Buffer too small
		FreeResource(hGlobal);
		return DefValue;
	}
	wchar_t Buffer[MAX_RESOURCE_VALUE_LENGTH];
	// Convert the multi-byte string to wide characters (UTF-8 assumed)
	int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, static_cast<LPCCH>(lpResourceData), resourceSize, Buffer, MAX_RESOURCE_VALUE_LENGTH);
	if (wideCharSize == 0) {
		// Conversion failed
		FreeResource(hGlobal);
		return DefValue;
	}
	// Null-terminate the string
	Buffer[wideCharSize] = L'\0';

	// Release the resource
	FreeResource(hGlobal);
	return std::wstring(Buffer);
}


Resources::Resources(HINSTANCE inst) {
	sjlPath = LoadStringFromResourceW(inst, L"SJL_PATH", L".sjl");
	jvmPath = LoadStringFromResourceW(inst, L"JVM_PATH", L"..\\..\\win-gui\\dist\\jdk");
	mutexName = LoadStringFromResourceW(inst, L"MUTEX_NAME", L"SJL-MUTEX");
	std::wstring arec = LoadStringFromResourceW(inst, L"INSTANCE_ALREADY_RUNNING_EXIT_CODE", L"0");
	instanceAlreadyRunningExitCode = std::stoi(arec);
	appTitle = LoadStringFromResourceW(inst, L"APP_TITLE", L"SJL-APP");
}

std::wstring Resources::GetSjlPath() {
	return sjlPath;
}

std::wstring Resources::GetJvmPath() {
	return jvmPath;
}

std::wstring Resources::GetMutexName() {
	return mutexName;
}

int Resources::GetInstanceAlreadyRunningExitCode() const {
	return instanceAlreadyRunningExitCode;
}

std::wstring Resources::GetAppTitle() {
	return appTitle;
}