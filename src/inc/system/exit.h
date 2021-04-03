#pragma once

#include "tools/platforms.h"

namespace exitHandler
{
	void initExitHandler();

	// may be a file or directory
	void add(const platform::string &a_wsPath);
	void remove(const platform::string &a_wsPath);
}