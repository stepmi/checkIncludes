#pragma once

#include <memory>
#include "compileFile/ICompileFile.h"

namespace compileFile
{
	std::unique_ptr<ICompileFile> readCompileFile(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy, const platform::string &a_wsProjectFile, const COMMANDLINE &a_commandLine);
	
}

