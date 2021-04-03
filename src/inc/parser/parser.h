#pragma once

#include <string>
#include "compileFile/CIncludeFile.h"

namespace parser
{	
	compileFile::INCLUDES findIncludes(const std::string &a_sCode);
}