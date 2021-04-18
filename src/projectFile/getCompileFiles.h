#pragma once

#include <string>
#include <vector>
#include "main/CParameters.h"
#include "compileFile/CCompileFileInfo.h"

namespace projectFile
{
	compileFile::COMPILE_FILES getCompileFiles(const CParameters &a_parameters, std::vector<std::string> &a_rRemovedCompileFiles);
}