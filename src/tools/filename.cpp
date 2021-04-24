#include "tools/filename.h"
#include <map>
#include <mutex>
#include "tools/strings.h"
#include <filesystem>
#include <chrono>
#include <cstdlib>
#include <random>
#include <limits>

namespace tools
{
	namespace filename
	{
		std::mutex mutex_files, mutex_random;
		std::map<std::string, std::string> global_files;
		std::mt19937 global_randomGenerator; 

		std::string getSpecificFileName(const std::string &a_sFileName)
		{
			std::lock_guard<std::mutex> guard(mutex_files);
			auto it = global_files.find(a_sFileName);
			if (it != global_files.end())
				return it->second;

			std::string sFilename = tools::strings::itos(global_files.size() + 1);
			if (sFilename.size() == 1)
				sFilename.insert(0, "00");
			else if (sFilename.size() == 2)
				sFilename.insert(0, "0");

			global_files.insert(std::make_pair(a_sFileName, sFilename));
			return sFilename;
		}

		int getRandom()
		{
			std::lock_guard<std::mutex> guard(mutex_random);
			std::uniform_int_distribution<int> distribution(0, std::numeric_limits<int>::max());
			return distribution(global_randomGenerator);
		}

		platform::string getTempFileName(const std::string &a_sExtension)
		{
			using TIME_POINT = std::chrono::time_point<std::chrono::system_clock>;
			const TIME_POINT tpNow = std::chrono::system_clock::now();
			auto iDuration = std::chrono::duration_cast<std::chrono::seconds>(tpNow.time_since_epoch()).count();
			const std::string sDuration = tools::strings::itos(iDuration);
			
			std::filesystem::path sFileName = "checkIncludes_" + sDuration + "_" + tools::strings::itos(getRandom()) + a_sExtension;
			return std::filesystem::temp_directory_path() / sFileName;
		}

		bool isIncludeFile(const std::string &a_sFileName)
		{
			const platform::string sFileName(a_sFileName);
			const std::string sExtension = sFileName.extension().string();
			return sExtension.size() > 1 && (sExtension[1] == 'h' || sExtension[1] == 'H');
		}

		bool hasDirectory(const std::string &a_sFileName)
		{
			const platform::string sFileName(a_sFileName);
			return sFileName.has_parent_path();
		}

	}

}