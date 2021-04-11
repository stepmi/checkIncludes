#pragma once

#include <string>

namespace execute
{
	enum class EResult
	{
		eOk,
		eFailed,	// program returned value != 0
		eError		// program couldn't even be started
	};

	EResult run(const std::string &a_sCommandline);
}