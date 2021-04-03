#include "tools/filename.h"
#include <map>
#include <mutex>
#include "tools/strings.h"

namespace tools
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

}