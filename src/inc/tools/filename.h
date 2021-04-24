#pragma once

#include <string>
#include "tools/platforms.h"

namespace tools
{
	namespace filename
	{
		std::string getSpecificFileName(const std::string &a_sFileName);

		platform::string getTempFileName(const std::string &a_sExtension);

		bool isIncludeFile(const std::string &a_sFileName);
		bool hasDirectory(const std::string &a_sFileName);

	}
}