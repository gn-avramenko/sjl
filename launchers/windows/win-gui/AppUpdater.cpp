#include "AppUpdater.h"
#include <vector>
#include <string>
#include "Utils.h"


struct UInfoFileMove {
	std::wstring source;
	std::wstring target;
};

struct UInfoFileDelete {
	std::wstring file;
};

struct UInfoSleep {
	int timeout;
};


enum UpdateOperationType {
	FILE_MOVE,
	FILE_DELETE,
	SLEEP
};

struct UpdateInfo {
	UpdateOperationType operationType;

	UInfoFileMove fileMove;
	UInfoFileDelete fileDelete;
	UInfoSleep sleep;
};

AppUpdater::AppUpdater(Locations *aLocations, ExceptionWrapper *ew, Resources *res, SplashScreen *splash, Debug *aDebug)
{
	locations = aLocations;
	debug = aDebug;
	exceptionWrapper = ew;
	resources = res;
	splashScreen = splash;
}

BOOL AppUpdater::IsUpdateRequired()
{
	return locations->FileExists(locations->GetUpdateFile());	
}

void AppUpdater::PerformUpdate()
{
	debug->Log("update script exist");
	std::vector<UpdateInfo> updateInfoList;

	FILE* f = NULL;
	int fo_res = _wfopen_s(&f, locations->GetUpdateFile().c_str(), L"rt, ccs=UTF-8");
	if (!fo_res)
	{
		wchar_t buffer[4096];
		while (fgetws(buffer, sizeof(buffer), f))
		{
			trim_line(buffer);
			std::wstring line = buffer;

			if (line.compare(L"file-move:") == 0)
			{
				UpdateInfo updateInfo;
				updateInfo.operationType = FILE_MOVE;


				if (fgetws(buffer, sizeof(buffer), f))
				{
					trim_line(buffer);
					updateInfo.fileMove.source = buffer;
				}

				if (fgetws(buffer, sizeof(buffer), f))
				{
					trim_line(buffer);
					updateInfo.fileMove.target = buffer;
				}

				updateInfoList.push_back(updateInfo);

			}
			else if (line.compare(L"file-delete:") == 0) {
				UpdateInfo updateInfo;
				updateInfo.operationType = FILE_DELETE;


				if (fgetws(buffer, sizeof(buffer), f)) {
					trim_line(buffer);
					updateInfo.fileDelete.file = buffer;
				}

				updateInfoList.push_back(updateInfo);
			}
			else if (line.compare(L"sleep:") == 0) {
				UpdateInfo updateInfo;
				updateInfo.operationType = SLEEP;

				// Parse the next line as the sleep timeout
				if (fgetws(buffer, sizeof(buffer), f)) {
					trim_line(buffer);
					updateInfo.sleep.timeout = std::stoi(buffer);
				}

				updateInfoList.push_back(updateInfo);
			}
		}
		fclose(f);
	}
	else
	{
		debug->Log("Unable to open update file");
		exceptionWrapper->ThrowException(format_message(L"unable to open update file %s", locations->GetSelfUpdateFile().c_str()), 
			format_message(resources->GetUnableToOpenFileMessage(), locations->GetUpdateFile().c_str()));
	}

	for (const UpdateInfo& updateInfo : updateInfoList) {

		switch (updateInfo.operationType) {
		case FILE_MOVE: {
			const UInfoFileMove& fileMove = updateInfo.fileMove;
			// Process fileMove.source and fileMove.target
			debug->Log(L"File Move: source %s target %s ", fileMove.source.c_str(), fileMove.target.c_str());
			if (!locations->FileExists(fileMove.source))
			{
				exceptionWrapper->ThrowException(format_message(L"source file %s does no exist", fileMove.source.c_str()), format_message(resources->GetSourceFileDoesNotExistMessage(), fileMove.source.c_str()));
			}
			if (locations->DirectoryExists(fileMove.source))
			{
				locations->CopyDirectory(fileMove.source, fileMove.target);
			}
			else
			{
				locations->FileCopy(fileMove.source, fileMove.target);
			}
			break;
		}
		case FILE_DELETE: {
			const UInfoFileDelete& fileDelete = updateInfo.fileDelete;
			debug->Log(L"File Delete: %s", fileDelete.file.c_str());
			if (!locations->FileExists(fileDelete.file))
			{
				debug->Log(L"File %s does not exist", fileDelete.file.c_str());
				continue;

			}
			if (locations->DirectoryExists(fileDelete.file))
			{
				locations->DirectoryRemove(fileDelete.file);
			}
			else
			{
				locations->FileDelete(fileDelete.file);
			}
			break;
		}
		case SLEEP: {
			const UInfoSleep& sleep = updateInfo.sleep;
			// Process sleep.timeout
			debug->Log(L"Sleep: %d seconds", sleep.timeout);
			int timeout = sleep.timeout;
			Sleep(timeout);
			break;
		}		
		}
	}
}
