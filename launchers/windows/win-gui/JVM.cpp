#include "JVM.h"
#include "jni.h"
#include "jni_md.h"
#include <vector>
#include <string>
#include "Utils.h"


void JVM::LaunchJVM()
{
	std::vector<std::string> vmOptionLines;
	std::wstring optionsFile = locations->GetOptionsFile();
	if (!optionsFile.empty())
	{
		debug->Log(L"reading options file %s", optionsFile.c_str());
		if (locations->FileExists(optionsFile))
		{
			debug->Log("file exists");
			FILE* f;
			int res = _wfopen_s(&f, optionsFile.c_str(), L"rt");
			if (!res && f)
			{
				char buffer[4096];
				while (fgets(buffer, sizeof(buffer), f))
				{
					trim_line(buffer);
					if (strlen(buffer) > 0 && buffer[0] != '#' && strcmp(buffer, "-server") != 0)
					{
						std::string line(buffer);
						debug->Log("added line %s", line);
						vmOptionLines.push_back(line);
					}
				}
				fclose(f);
			}
			else
			{
				exceptionWrapper->ThrowException(format_message(L"unable to open file %s, code = %s", optionsFile.c_str(), res), format_message(resources->GetUnableToOpenFileMessage(), optionsFile.c_str()));
			}
		}
	}


	JavaVMInitArgs vmArgs;
	vmArgs.version = JNI_VERSION_1_2;
	//vmArgs.nOptions = vmOptionsCount;
	//vmArgs.options = vmOptions;
	vmArgs.ignoreUnrecognized = JNI_TRUE;
}
