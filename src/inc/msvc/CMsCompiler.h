#pragma once

#include "compiler/ICompiler.h"

namespace msvc
{	
	std::unique_ptr<compiler::ICompiler> createMsCompiler();

}