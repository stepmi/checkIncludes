#pragma once

#include "tools/interface.h"

namespace compiler
{
	ci_interface ICompiler;
}

class CParameters;

namespace projectFile
{	
	void checkProject(const compiler::ICompiler &a_compiler, const CParameters &a_parameters);
}
