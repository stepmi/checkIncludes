#pragma once

#include <string>
#include "compileFile/CIncludeFile.h"
#include "compileFile/CIncludeFileIgnore.h"

namespace compileFile
{
	void filterIncludes(const std::string &a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore, INCLUDES &a_includes);
}
