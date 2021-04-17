#include "make/CMakeFile.h"
#include "system/execute.h"
#include "system/logger.h"
#include "tools/strings.h"
#include <filesystem>
#include "tools//filesystem.h"

namespace make
{
	std::vector<std::string> getMakeCommandLinesFromMakeResult(const std::string &a_sMakeResult)
	{
		return {};
	}

	std::vector<std::string> getMakeCommandLines(const std::string &a_sMakeFile)
	{
#ifdef _WIN32
		const std::string sMakePath = execute::getCommandPath("make.exe");
#else
		const std::string sMakePath = "make";
#endif

		std::filesystem::path sWorkingDir;
		{
			std::filesystem::path sMakeFilePath(a_sMakeFile);
			if (sMakeFilePath.has_parent_path())
				sWorkingDir = sMakeFilePath.parent_path();
		}

		//options:
		// -B to ignore timestamps aka force build
		// -n to skip build, but only output the commands
		const std::string sCommandLine = sMakePath + " -B -n ";
		std::string sMakeResult;
		auto eResult = execute::runOutputToString(sCommandLine, sWorkingDir, sMakeResult);
		if (eResult == execute::EResult::eError)
			logger::add(logger::EType::eError, "Error: Couldn't start " + tools::strings::getQuoted(sCommandLine));
		else if (eResult == execute::EResult::eFailed)
			logger::add(logger::EType::eError, "Error: " + tools::strings::getQuoted(sCommandLine) + " returned an error.");
		else
		{
#ifndef _WIN32
			//tools::filesystem::writeFile("./make_result_linux.txt", sMakeResult);
#endif
			if (sMakeResult.empty())
				logger::add(logger::EType::eError, "Error: make result not found.");
			else
			{
				return getMakeCommandLinesFromMakeResult(sMakeResult);
			}
		}

		return {};

	}

}
