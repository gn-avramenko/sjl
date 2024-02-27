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
	unableToOpenFileMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_OPEN_FILE_MESSAGE", L"Unable to open file %s");
	unableToCopyDirectoryMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_COPY_DIRECTORY_MESSAGE", L"Unable to copy directory from %s to %s");
	unableToCreateDirectoryMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_CREATE_DIRECTORY_MESSAGE", L"Unable to create directory %s");
	sourceFileDoesNotExistMessage = LoadStringFromResourceW(inst, L"SOURCE_FILE_DOES_NOT_EXIST_MESSAGE", L"Source file %s does not exist");
	unableToCopyFileMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_COPY_FILE_MESSAGE", L"Unable to copy file from %s to %s");
	unableToDeleteDirectoryMessage= LoadStringFromResourceW(inst, L"UNABLE_TO_DELETE_DIRECTORY_MESSAGE", L"Unable to delete %s");
	unableToDeleteFileMessage= LoadStringFromResourceW(inst, L"UNABLE_TO_DELETE_FILE_MESSAGE", L"Unable to delete %s");
	unableToRenameFileMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_RENAME_FILE_MESSAGE", L"Unable to rename file %s to %s");
	unableToPerformSelfUpdateMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_PERFORM_SELF_UPDATE_MESSAGE", L"Unable to perform self update");
	unableToLoadBitmapMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_LOAD_BITMAP_MESSAGE", L"Unable to load bitmap from %s");
	splashScreenFileName = LoadStringFromResourceW(inst, L"SPLASH_SCREEN_FILE", std::wstring());
	optionsFile = LoadStringFromResourceW(inst, L"OPTIONS_FILE", std::wstring());
	errorMessage = LoadStringFromResourceW(inst, L"ERROR_MESSAGE", L"Error");
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

std::wstring Resources::GetUnableToOpenFileMessage()
{
	return unableToOpenFileMessage;
}

std::wstring Resources::GetUnableToCreateDirectoryMessage()
{
	return unableToCreateDirectoryMessage;
}

std::wstring Resources::GetUnableToCopyFileMessage()
{
	return unableToCopyFileMessage;
}

std::wstring Resources::GetUnableToDeleteDirectoryMessage()
{
	return unableToDeleteDirectoryMessage;
}

std::wstring Resources::GetUnableToDeleteFileMessage()
{
	return unableToDeleteFileMessage;
}

std::wstring Resources::GetUnableToRenameFileMessage()
{
	return unableToRenameFileMessage;
}

std::wstring Resources::GetUnableToPerformSelfUpdateMessage()
{
	return unableToPerformSelfUpdateMessage;
}

std::wstring Resources::GetErrorMessage()
{
	return errorMessage;
}

std::wstring Resources::GetUnableToLoadBitmapMessage()
{
	return unableToLoadBitmapMessage;
}

std::wstring Resources::GetSplashScreenFile()
{
	return splashScreenFileName;
}

std::wstring Resources::GetOptionsFile()
{
	return optionsFile;
}

std::wstring Resources::GetSourceFileDoesNotExistMessage()
{
	return sourceFileDoesNotExistMessage;
}

std::wstring Resources::GetUnableToCopyDirectoryMessage()
{
	return unableToCopyDirectoryMessage;
}

