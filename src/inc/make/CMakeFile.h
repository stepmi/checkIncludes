#pragma once

#include <vector>
#include <string>
#include "compileFile/CCompileFileInfo.h"

namespace make
{		
	compileFile::COMPILE_FILES getCompileFiles(const std::string &a_sMakeFile);
}