#include "system/logger.h"
#include <iostream>
#include <mutex>

namespace logger
{
	constexpr bool bShowDiagnose = false;
	constexpr bool bShowDiagnoseThreads = false;

	std::mutex mutex_logger;

	bool getIgnore(const EType a_eType)
	{
		if (!bShowDiagnose && a_eType == EType::eDiagnose)
			return true;
		if (!bShowDiagnoseThreads && a_eType == EType::eDiagnoseThreads)
			return true;
		return false;
	}
	
	void add(const EType a_eType, const std::string &a_sText)
	{
		std::lock_guard<std::mutex> guard(mutex_logger);
		if (getIgnore(a_eType))
			return;
		std::cout << a_sText << "\n";
	}

	void add(const EType a_eType, const std::vector<std::string> &a_Texts)
	{
		std::lock_guard<std::mutex> guard(mutex_logger);
		if (getIgnore(a_eType))
			return;
		for (auto &sText : a_Texts)
			std::cout << sText << "\n";
	}
}