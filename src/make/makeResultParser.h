#pragma once

#include <string>
#include <vector>
#include "make/CMakeFile.h"

namespace make
{	
	COMMANDLINES getCommandLinesFromMakeResult(const std::string &a_sMakeResult);
}