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

		const std::filesystem::path sResultFile = sWorkingDir / "checkIncludes_make_result.txt";
		// TODO: this doesn't work. We have to find another method to redirect the output.
		const std::string sCommandLine = sMakePath + " -n "; // > " + sResultFile.string();
		
		auto eResult = execute::run(sCommandLine, sWorkingDir);
		if (eResult == execute::EResult::eError)
			logger::add(logger::EType::eError, "Error: Couldn't start " + tools::strings::getQuoted(sCommandLine));
		else if (eResult == execute::EResult::eFailed)
			logger::add(logger::EType::eError, "Error: " + tools::strings::getQuoted(sCommandLine) + " returned an error.");
		else
		{
			const std::string sMakeResult = tools::filesystem::readFile(sResultFile);
			if (sMakeResult.empty())
				logger::add(logger::EType::eError, "Error: make result not found " + tools::strings::getQuoted(sResultFile.string()));
			else
			{
				return getMakeCommandLinesFromMakeResult(sMakeResult);
			}
		}

		return {};

	}

}