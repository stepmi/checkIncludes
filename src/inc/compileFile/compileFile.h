#pragma once

#include "tools/platforms.h"
#include "compileFile/CIncludeFileIgnore.h"
#include "tools/interface.h"

namespace projectFile
{
	ci_interface IThread;
}

class CParameters;

namespace compileFile
{
	void checkCompileFile(projectFile::IThread &a_thread, const CParameters &a_parameters, const std::string a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore);
	
}

