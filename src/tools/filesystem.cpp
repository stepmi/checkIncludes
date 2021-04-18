#include "tools/filesystem.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace tools
{
	namespace filesystem
	{
		std::string readFile(const platform::string &a_wsFile)
		{
			const std::ifstream inputStream(a_wsFile, std::ifstream::in | std::ios_base::binary);
			if (!inputStream.fail())
			{
				std::stringstream sBuffer;
				sBuffer << inputStream.rdbuf();
				return sBuffer.str();
			}
			return std::string();
		}
		
		bool writeFile(const platform::string &a_wsFile, const std::string &a_sContent)
		{
			std::ofstream outputStream(a_wsFile, std::ifstream::out | std::ios_base::binary);
			if (!outputStream.fail())
			{
				outputStream << a_sContent;
				return true;
			}
			return false;
		}

		bool removeAll(const platform::string &a_wsPath)
		{
			std::error_code errorCode;
			return std::filesystem::remove_all(a_wsPath, errorCode);
		}

		bool copyFile(const platform::string &a_wsSrc, const platform::string &a_wsDest)
		{
			std::error_code errorCode;
			return std::filesystem::copy_file(a_wsSrc, a_wsDest, std::filesystem::copy_options::overwrite_existing, errorCode);
		}

		platform::string resolveSymLink(const platform::string &a_wsPath)
		{
			std::error_code errorCode;
			platform::string sPath = a_wsPath;
			while (std::filesystem::is_symlink(sPath, errorCode))
			{
				sPath = std::filesystem::read_symlink(sPath, errorCode);
				if (errorCode)
					return platform::string();
				else if(sPath == a_wsPath)
					return platform::string(); // this was a circular symlink
			}
			return sPath;
		}

	}

}