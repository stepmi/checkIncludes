#include "tools/filename.h"
#include <map>
#include <mutex>
#include "tools/strings.h"

namespace tools
{
	namespace filename
	{
		std::mutex mutex_files;
		std::map<std::string, std::string> global_files;

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