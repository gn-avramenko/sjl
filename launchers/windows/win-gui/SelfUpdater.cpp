#include "SelfUpdater.h"
#include <string>
#include "Utils.h"

struct SelfUpdateInfo {
	std::wstring currentPath;
	std::wstring updatedPath;
};

SelfUpdater::SelfUpdater(Locations* locs, Resources* res, ExceptionWrapper* ew, Debug* deb)
{
	locations = locs;
	debug = deb;
	exceptionWrapper = ew;
	resources = res;

}

BOOL SelfUpdater::IsUpdateRequired()
{
	return locations->FileExists(locations->GetSelfUpdateFile());
}

void SelfUpdater::PerformUpdate()
{
	debug->Log("performing self update");
	SelfUpdateInfo selfupdateInfo;

	FILE* f = NULL;
	int fo_res = _wfopen_s(&f, locations->GetSelfUpdateFile().c_str(), L"rt, ccs=UTF-8");
	if (!fo_res)
	{
		wchar_t buffer[4096];


		if (fgetws(buffer, sizeof(buffer), f))
		{
			trim_line(buffer);
			selfupdateInfo.currentPath = buffer;
		}

		if (fgetws(buffer, sizeof(buffer), f))
		{
			trim_line(buffer);
			selfupdateInfo.updatedPath = buffer;
		}
		fclose(f);
	}
	else
	{
		exceptionWrapper->ThrowException(format_message(L"unable to open file %s, code %s", locations->GetSelfUpdateFile().c_str(), fo_res),
			format_message(resources->GetUnableToOpenFileMessage(), locations->GetSelfUpdateFile().c_str()));
	}
	WCHAR currentFilePath[MAX_PATH];
	swprintf_s(currentFilePath, L"%s", selfupdateInfo.currentPath.c_str());
	WCHAR updateFilePath[MAX_PATH];
	swprintf_s(updateFilePath, L"%s%s", selfupdateInfo.currentPath.c_str(), L".del");
	// Rename the file
	SHFILEOPSTRUCTW fileOp;
	ZeroMemory(&fileOp, sizeof(fileOp));
	fileOp.wFunc = FO_RENAME;
	fileOp.pFrom = currentFilePath;
	fileOp.pTo = updateFilePath;
	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;

	int renameResult = SHFileOperationW(&fileOp);
	if (renameResult != 0) {
		exceptionWrapper->ThrowException(format_message(L"Unable to rename file %s to %s, code %d", std::wstring(fileOp.pFrom).c_str(), std::wstring(fileOp.pFrom).c_str(), renameResult),
			format_message(resources->GetUnableToRenameFileMessage(), std::wstring(fileOp.pFrom).c_str(), std::wstring(fileOp.pFrom).c_str()));
	}


	WCHAR tempDir[MAX_PATH];
	GetTempPathW(MAX_PATH, tempDir);

	WCHAR tempFilePath[MAX_PATH];
	swprintf_s(tempFilePath, L"%s\\sjl_tmp_del.exe", tempDir);

	// Delete the original file
	BOOL moveResult = MoveFileExW(updateFilePath, tempFilePath, MOVEFILE_REPLACE_EXISTING);
	if (!moveResult) {
		exceptionWrapper->ThrowException(format_message(L"Unable to move file %s to %s", std::wstring(updateFilePath).c_str(), std::wstring(tempFilePath).c_str()),
			format_message(resources->GetUnableToRenameFileMessage(), std::wstring(updateFilePath).c_str(), std::wstring(tempFilePath).c_str()));
	}

	if (CopyFileW(selfupdateInfo.updatedPath.c_str(), selfupdateInfo.currentPath.c_str(), FALSE)) {
		debug->Log(L"Self-update successful. Restarting...");

		locations->DirectoryRemove(locations->GetUpdateDirectory());
		
		// Restart the application by launching the updated executable
		ShellExecute(nullptr, L"open", selfupdateInfo.currentPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

		// Exit the current instance of the application
		exit(0);
	}
	exceptionWrapper->ThrowException(L"unable to perform self update", resources->GetUnableToPerformSelfUpdateMessage());
}
