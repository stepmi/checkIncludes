#pragma once

#include <string>
#include "tools/platforms.h"

namespace execute
{
	enum class EResult
	{
		eOk,
		eFailed,	// program returned value != 0
		eError		// program couldn't even be started
	};
		
	// use current working dir, if a_sWorkingDir is empty
	EResult run(const std::string &a_sCommandline, const platform::string &a_sWorkingDir);

	std::string getCommandPath(const std::string &a_sCommand);
}