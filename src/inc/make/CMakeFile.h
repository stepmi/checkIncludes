#pragma once

#include <vector>
#include <string>

namespace make
{
	using COMMANDLINE = std::vector<std::string>;
	using COMMANDLINES = std::vector<COMMANDLINE>;

	COMMANDLINES getMakeCommandLines(const std::string &a_sMakeFile);
}