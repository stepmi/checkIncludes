#pragma once

#include <vector>
#include <string>

namespace make
{
	std::vector<std::string> getMakeCommandLines(const std::string &a_sMakeFile);
}