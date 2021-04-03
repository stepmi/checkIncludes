#pragma once

#include <memory>
#include "compileFile/ICompileFile.h"
#include "compileFile/CIncludeFileIgnore.h"

namespace compileFile
{
	std::unique_ptr<ICompileFile> readCompileFile(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy,
		const platform::string &a_wsProjectFile, const std::string &a_sStdAfx, INCLUDES_TO_IGNORE &a_includesToIgnore);
	
}

