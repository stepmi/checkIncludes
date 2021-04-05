#include "system/logger.h"
#include <iostream>
#include <mutex>
#include <set>
#include "tools/find.h"

namespace logger
{	
	std::mutex mutex_logger;

	std::set<EType> global_allowedTypes = { EType::eError, EType::eMessage, EType::eWarning };

	bool isTypeAllowed(const EType a_eType)
	{
		return tools::find(global_allowedTypes, a_eType);		
	}

	void allowType(const EType a_eType)
	{
		std::lock_guard<std::mutex> guard(mutex_logger);
		global_allowedTypes.insert(a_eType);
	}
	
	void add(const EType a_eType, const std::string &a_sText)
	{
		std::lock_guard<std::mutex> guard(mutex_logger);
		if (isTypeAllowed(a_eType))		
		std::cout << a_sText << "\n";
	}

	void add(const EType a_eType, const std::vector<std::string> &a_Texts)
	{
		std::lock_guard<std::mutex> guard(mutex_logger);
		if (isTypeAllowed(a_eType))
		{
			for (auto &sText : a_Texts)
				std::cout << sText << "\n";
		}
	}
}