#include "SelfUpdater.h"
#include <string>
#include "Utils.h"


SelfUpdater::SelfUpdater(Locations* locs, Resources* res, PWSTR args,  ExceptionWrapper* ew, Debug* deb)
{
	locations = locs;
	debug = deb;
	exceptionWrapper = ew;
	resources = res;
	programParams = args;
}

BOOL SelfUpdater::IsUpdateRequired()
{	
	std::wstring params = std::wstring(programParams);
	if (params.find(L"-sjlu1") != std::wstring::npos) {
		return true;
	}
	if (params.find(L"-sjlu2") != std::wstring::npos) {
		return true;
	}
	return locations->FileExists(locations->GetSelfUpdateFile());
}


std::wstring SelfUpdater::getCurrentLauncherFileName()
{
	std::wstring path = locations->GetBasePath() + L"\\current-launcher-path.txt";
	std::wstring result = locations->ReadFileContent(path);
	if (result.length() == 0) {
		exceptionWrapper->ThrowException(format_message(L"content of file %s is empty", path.c_str()), resources->GetUnableToPerformSelfUpdateMessage());
	}
	return result;
}

std::wstring SelfUpdater::getNewLauncherFileName(BOOL updateStarted)
{
	if (updateStarted) {
		return locations->GetExecutablePath();
	}
	std::wstring result = locations->ReadFileContent(locations->GetSelfUpdateFile());
	if (result.length() == 0) {
		exceptionWrapper->ThrowException(format_message(L"content of file %s is empty", locations->GetSelfUpdateFile().c_str()), resources->GetUnableToPerformSelfUpdateMessage());
	}
	return result;	
}

void SelfUpdater::PerformUpdate()
{
	std::wstring params = std::wstring(programParams);
	if (params.find(L"-sjlu1") != std::wstring::npos) {
		debug->Log(L"flag -sjlu1 exists");
		Sleep(500);
		std::wstring currentLauncherFileName = getCurrentLauncherFileName();
		debug->Log(L"current launcher file name is %s", currentLauncherFileName.c_str());
		locations->FileDelete(currentLauncherFileName);
		locations->FileCopy(getNewLauncherFileName(true), currentLauncherFileName);
		params = replace(params, L"-sjlu1", L"");
		params = params + L" -sjlu2";
		std::wstring currentLauncher = getCurrentLauncherFileName();
		debug->Log(L"executing command %s %s", currentLauncher.c_str(), params.c_str());
		ShellExecuteW(NULL, L"open", currentLauncher.c_str(), params.c_str(), NULL, SW_RESTORE);
		exit(0);
		return;
	}
	if (params.find(L"-sjlu2") != std::wstring::npos) {
		debug->Log(L"flag -sjlu2 exists");
		Sleep(500);
		return;
	}
	if (locations->FileExists(locations->GetSelfUpdateFile())) {
		debug->Log(L"self update file %s exists", locations->GetSelfUpdateFile().c_str());
		std::wstring newLauncherFileName = getNewLauncherFileName(false);
		size_t position = newLauncherFileName.find_last_of(L"\\");
		std::wstring basePath = newLauncherFileName.substr(0, position);
		locations->WriteFileContent(basePath + L"\\current-launcher-path.txt", locations->GetExecutablePath());
		params = params + L" -sjlu1";
		debug->Log(L"executing command %s %s", newLauncherFileName.c_str(), params.c_str());
		ShellExecuteW(NULL, L"open", newLauncherFileName.c_str(), params.c_str(), NULL, SW_RESTORE);
		exit(0);
		return;
	}
    exceptionWrapper->ThrowException(L"unable to perform self update", resources->GetUnableToPerformSelfUpdateMessage());
}
