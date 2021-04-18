#pragma once

#include <string>
#include <vector>
#include "make/CMakeFile.h"
#include "compileFile/CCompileFileInfo.h"

namespace make
{	
	using COMMANDLINES = std::vector<compileFile::COMMANDLINE>;

	COMMANDLINES getCommandLinesFromMakeResult(const std::string &a_sMakeResult);
}