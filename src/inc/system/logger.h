#pragma once

#include <string>
#include <vector>

namespace logger
{
	enum class EType
	{
		eError,
		eWarning,
		eMessage,
		eProcessedFiles,
		eCheckedIncludes,
		eCommandLines,		
		eDebugThreads
	};
	
	void allowType(const EType a_eType);
	void add(const EType a_eType, const std::string &a_sText);
	void add(const EType a_eType, const std::vector<std::string> &a_Texts); // add multiple lines without being interrupted by other threads
}
