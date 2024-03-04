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


static std::string LoadStringFromResource(HINSTANCE hInstance, const wchar_t* ResourceName, std::string DefValue)
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
	char Buffer[MAX_RESOURCE_VALUE_LENGTH];
	strcpy_s(Buffer, static_cast<char*>(lpResourceData));


	// Null-terminate the string
	Buffer[resourceSize] = '\0';

	// Release the resource
	FreeResource(hGlobal);
	return std::string(Buffer);
}


Resources::Resources(HINSTANCE inst) {
	sjlPath = LoadStringFromResourceW(inst, L"SJL_PATH", L".sjl");
	mutexName = LoadStringFromResourceW(inst, L"MUTEX_NAME", L"SJL-MUTEX");
	std::wstring arec = LoadStringFromResourceW(inst, L"INSTANCE_ALREADY_RUNNING_EXIT_CODE", L"0");
	instanceAlreadyRunningExitCode = std::stoi(arec);
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
	unableToLocateJvmDllMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_LOCATE_JVM_DLL_MESSAGE", L"Unable to locate jvm.dll in %s");
	unableToLoadJvmDllMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_LOAD_JVM_DLL_MESSAGE", L"Unable to load JVM from %s");
	unableToFindAdressOfJNI_CreateJavaVMMessage= LoadStringFromResourceW(inst, L"UNABLE_TO_FIND_ADDRESS_OF_CREATE_JVM_MESSAGE", L"Unable to find address of JNI_CreateJavaVM");
	classPathIsNotDefinedMessage = LoadStringFromResourceW(inst, L"CLASS_PATH_IS_NOT_DEFINED_MESSAGE", L"Classpath is not defined");
	splashScreenFileName = LoadStringFromResourceW(inst, L"SPLASH_SCREEN_FILE", L"..\\..\\..\\..\\examples\\sample-gui-app\\src\\main\\resources\\splash.bmp");
	vmOptionsFile = LoadStringFromResourceW(inst, L"VM_OPTIONS_FILE", L"..\\..\\..\\..\\examples\\sample-gui-app\\src\\main\\resources\\win-gui.options");
	vmOptions = LoadStringFromResource(inst, L"VM_OPTIONS", "-Xms128m|-Xmx??256m");
	embeddedJavaHomePath = LoadStringFromResourceW(inst, L"EMBEDDED_JAVA_HOME", L"..\\..\\..\\..\\examples\\sample-gui-app\\dist\\jdk");
	classPath = LoadStringFromResource(inst, L"CLASS_PATH", "..\\..\\..\\..\\examples\\sample-gui-app\\dist\\sample-gui-app.jar");
	restartExitCode = std::stoi(LoadStringFromResource(inst, L"RESTART_EXIT_CODE", "79"));
	errorTitle = LoadStringFromResourceW(inst, L"ERROR_TITLE", L"Error");
	unableToCreateJVMMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_CREATE_JVM_MESSAGE", L"Unable to create JVM");
	unableToFindMainClassMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_FIND_MAIN_CLASS_MESSAGE", L"Unable to find main class %s");
	instanceAlreadyRunningMessage = LoadStringFromResourceW(inst, L"INSTANCE_ALREADY_RUNNING_MESSAGE", L"Instance of the application is already running");
	unableToFindMainMethodMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_FIND_MAIN_METHOD_MESSAGE", L"Unable to find main method");
	errorInvokingMainMethodMessage = LoadStringFromResourceW(inst, L"ERROR_INVOKING_MAIN_METHOD_MESSAGE", L"Exception occured while invoking main method");
	javaHomeIsNotDefinedMessage = LoadStringFromResourceW(inst, L"JAVA_HOME_IS_NOT_DEFINED_MESSAGE", L"JAVA_HOME environment variable is not defined");
	unableToCheckInstalledJavaMessage = LoadStringFromResourceW(inst, L"UNABLE_TO_CHECK_INSTALLED_JAVA_MESSAGE", L"Unable to check installed java");
	wrongJavaTypeMessage = LoadStringFromResourceW(inst, L"WRONG_JAVA_TYPE_MESSAGE", L"Wrong Java Version: required %s min version=%d max version %d, found %s version = %d");
	useInstalledJava = L"true" == LoadStringFromResourceW(inst, L"USE_INSTALLED_JAVA", L"false");
	required64JRE = L"true" == LoadStringFromResourceW(inst, L"REQUIRES_64_BIT", L"true");
	minJavaVersion = std::stoi(LoadStringFromResource(inst, L"MIN_JAVA_VERSION", "0"));
	maxJavaVersion = std::stoi(LoadStringFromResource(inst, L"MAX_JAVA_VERSION", "0"));
	mainClass = LoadStringFromResource(inst, L"MAIN_CLASS", "com/gridnine/sjl/example/winGui/WinGui");
}

std::wstring Resources::GetSjlPath() {
	return sjlPath;
}


std::wstring Resources::GetMutexName() {
	return mutexName;
}

int Resources::GetInstanceAlreadyRunningExitCode() const {
	return instanceAlreadyRunningExitCode;
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

std::wstring Resources::GetErrorTitle()
{
	return errorTitle;
}

std::wstring Resources::GetUnableToLoadBitmapMessage()
{
	return unableToLoadBitmapMessage;
}

std::wstring Resources::GetSplashScreenFile()
{
	return splashScreenFileName;
}

std::wstring Resources::GetVMOptionsFile()
{
	return vmOptionsFile;
}

std::string Resources::GetVMOptions()
{
	return vmOptions;
}

std::wstring Resources::GetEmbeddedJavaHomePath()
{
	return embeddedJavaHomePath;
}

std::wstring Resources::GetUnableToLocateJvmDllMessage()
{
	return unableToLocateJvmDllMessage;
}

std::string Resources::GetClassPath()
{
	return classPath;
}

std::wstring Resources::GetClassPathIsNotDefinedMessage()
{
	return classPathIsNotDefinedMessage;
}

std::wstring Resources::GetUnableToLoadJvmDllMessage()
{
	return unableToLoadJvmDllMessage;
}

std::wstring Resources::GetUnableToFindAdressOfJNI_CreateJavaVMMessage()
{
	return unableToFindAdressOfJNI_CreateJavaVMMessage;
}

int Resources::GetRestartExitCode()
{
	return restartExitCode;
}

std::wstring Resources::GetUnableToCreateJVMMessage()
{
	return unableToCreateJVMMessage;
}

std::wstring Resources::GetUnableToFindMainClassMessage()
{
	return unableToFindMainClassMessage;
}

std::wstring Resources::GetUnableToFindMainMethodMessage()
{
	return unableToFindMainMethodMessage;
}

std::wstring Resources::GetErrorInvokingMainMethodMessage()
{
	return errorInvokingMainMethodMessage;
}

std::wstring Resources::GetInstanceAlreadyRunningMessage()
{
	return instanceAlreadyRunningMessage;
}

std::wstring Resources::GetJavaHomeIsNotDefinedMessage()
{
	return javaHomeIsNotDefinedMessage;
}

std::wstring Resources::GetWrongJavaTypeMessage()
{
	return wrongJavaTypeMessage;
}

std::wstring Resources::GetUnableToCheckInstalledJavaMessage()
{
	return unableToCheckInstalledJavaMessage;
}

std::string Resources::GetMainClass()
{
	return mainClass;
}

bool Resources::IsUseInstalledJava()
{
	return useInstalledJava;
}

bool Resources::IsRequired64JRE()
{
	return required64JRE;
}

int Resources::GetMinJavaVersion()
{
	return minJavaVersion;
}

int Resources::GetMaxJavaVersion()
{
	return maxJavaVersion;
}

std::wstring Resources::GetSourceFileDoesNotExistMessage()
{
	return sourceFileDoesNotExistMessage;
}

std::wstring Resources::GetUnableToCopyDirectoryMessage()
{
	return unableToCopyDirectoryMessage;
}

