#include "CGppCompiler.h"
#include <filesystem>
#include "tools/strings.h"

namespace compiler
{

	bool getIsCompileFile(const std::string &a_sArgument)
	{
		const auto sExtension = std::filesystem::path(a_sArgument).extension().string();
		// see g++ man
		// we don't differentiate for c or c++
		const std::vector<std::string> extensions = { ".cc", ".cp", ".cxx", ".cpp", ".c++", "c" };
		for (auto &sExtensionCandidate : extensions)
		{			
			if (tools::strings::compareCaseInsensitive(sExtension, sExtensionCandidate))
				return true;
		}
		return false;
	}

	std::string CGppCompiler::getCompileFileFromCommandLine(const compileFile::COMMANDLINE &a_commandline) const
	{	
		// TODO: Is it possible, that there's more than one compile file, created with one command?
		// Then we'd have to split the calls up into one command for each compile file.
		for (const auto &sArgument : a_commandline)
		{
			if (&sArgument != &a_commandline.front())
			{
				if (getIsCompileFile(sArgument))
					return sArgument;
			}
		}
		return std::string();
	}

}