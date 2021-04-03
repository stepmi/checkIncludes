#pragma once

#include "tools/interface.h"

namespace projectFile
{
	ci_interface IThread
	{
		virtual ~IThread() = default;
		virtual void setIsFinished() = 0;
	};

}
