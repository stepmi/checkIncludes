#pragma once

#include "tools/platforms.h"
#include "tools/interface.h"
#include "includeFileHandle.h"
#include "CIncludeFileIgnore.h"
#include "compileFile/CCompileFileInfo.h"

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
		virtual const COMMANDLINE &getCommandLine() const = 0;
		virtual INCLUDE_HANDLES getIncludesToCheck() const = 0; // returns only includes that aren't set to ignore
		virtual INCLUDE_HANDLES getIncludes() const = 0; // returns all includes
		virtual const CInclude *getInclude(const HANDLE_INCLUDE a_hInclude) const = 0;		
		
		virtual bool switchInclude(const HANDLE_INCLUDE a_hInclude, const bool a_bSwitchOn) = 0;				
		virtual void filterIncludes(INCLUDES_TO_IGNORE &a_includesToIgnore, const platform::string &a_sPreProcessFile) = 0;

		// preprocess
		virtual bool addMarkersForPreProcess() = 0;
		virtual bool removeMarkersForPreProcess() = 0;		
	};
	
	

}