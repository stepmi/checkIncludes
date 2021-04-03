#pragma once

#include <string>
#include "tools/interface.h"
#include "tools/platforms.h"

namespace projectFile
{
	// a project file (like .vcxproj for MsVc)
	ci_interface IProject
	{
		virtual ~IProject() = default;

		virtual std::string getStdAfx() const = 0;
		virtual const platform::string &getProjectFileWorkingCopy() const = 0;
		virtual const std::string &getCompileFileWorkingCopy() const = 0;
	};
}