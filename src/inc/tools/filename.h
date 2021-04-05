#pragma once

#include <string>

namespace tools
{
	namespace filename
	{
		std::string getSpecificFileName(const std::string &a_sFileName);

		bool isIncludeFile(const std::string &a_sFileName);
		bool hasDirectory(const std::string &a_sFileName);

	}
}