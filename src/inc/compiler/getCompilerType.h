#pragma once

#include "main/ECompilerType.h"
#include <string>

namespace compiler
{

	ECompilerType getCompilerTypeFromCommand(const std::string &a_sCommand);

}