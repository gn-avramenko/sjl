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
	sjl_path = LoadStringFromResourceW(inst, L"SJL_PATH", L".sjl");
	jvm_path = LoadStringFromResourceW(inst, L"JVML_PATH", L"..\\..\\win-gui\\dist\\jdk");
}

std::wstring Resources::GetSjlPath() {
	return sjl_path;
}

std::wstring Resources::GetJvmPath() {
	return jvm_path;
}
