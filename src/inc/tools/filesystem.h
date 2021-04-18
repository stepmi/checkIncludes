#pragma once

#include "platforms.h"

namespace tools
{
	namespace filesystem
	{
		std::string readFile(const platform::string &a_wsFile);
		bool writeFile(const platform::string &a_wsFile, const std::string &a_sContent);

		bool removeAll(const platform::string &a_wsPath); // may be a file or directory
		bool copyFile(const platform::string &a_wsSrc, const platform::string &a_wsDest);		

		platform::string resolveSymLink(const platform::string &a_wsPath);
	}

}