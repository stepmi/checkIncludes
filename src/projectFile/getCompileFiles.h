#pragma once

#include <string>
#include <vector>
#include "main/CParameters.h"

namespace projectFile
{
	std::vector<std::string> getCompileFiles(const CParameters &a_parameters);
}