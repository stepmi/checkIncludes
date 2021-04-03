#pragma once

#include "tools/platforms.h"
#include "tools/interface.h"
#include "includeFileHandle.h"

namespace compileFile
{
	class CInclude;

	// a file from the project, that can be compiled. Most likely a .cpp or .c file.
	ci_interface ICompileFile
	{
	public:
		virtual ~ICompileFile() = default;

		virtual const std::string &getFileWorkingCopy() const = 0;
		virtual const std::string &getFile() const = 0; // the original filename
		virtual const platform::string &getProjectFileWorkingCopy() const = 0;
		virtual INCLUDE_HANDLES getIncludes() const = 0;
		virtual const CInclude *getInclude(const HANDLE_INCLUDE a_hInclude) const = 0;
		
		virtual bool switchInclude(const HANDLE_INCLUDE a_hInclude, const bool a_bSwitchOn) = 0;		
	};
	
	

}