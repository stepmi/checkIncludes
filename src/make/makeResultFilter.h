#pragma once

#include "compileFile/CCompileFileInfo.h"
#include "makeResultParser.h"

namespace make
{
	compileFile::COMPILE_FILES filterCommandLines(const COMMANDLINES &a_commandLines);
}