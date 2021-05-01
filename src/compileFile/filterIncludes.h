#pragma once

#include <string>
#include "compileFile/CIncludeFile.h"
#include "compileFile/CIncludeFileIgnore.h"

namespace compileFile
{
	void filterIncludesByIncludesToIgnore(const std::string &a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore, INCLUDES &a_includes);
	void filterIncludesWithCompileFileName(const std::string &a_sCompileFile, INCLUDES &a_includes);
	void filterIncludesByPreProcessResult(const std::string &a_sPreProcessResult, INCLUDES &a_includes);
}
