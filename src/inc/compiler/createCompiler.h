#pragma once

#include "ICompiler.h"
#include "main/ECompilerType.h"
#include <memory>

namespace compiler
{

	std::unique_ptr<ICompiler> createCompiler(const ECompilerType a_eType);

}