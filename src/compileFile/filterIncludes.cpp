#include "filterIncludes.h"
#include "tools/strings.h"
#include <mutex>
#include <algorithm>
#include "tools/filesystem.h"

namespace compileFile
{
	std::mutex mutex_includesToIgnore;

	void filterIncludesByIncludesToIgnore(const std::string &a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore, INCLUDES &a_includes)
	{
		std::lock_guard<std::mutex> guard(mutex_includesToIgnore);
		for (auto &include : a_includes)
		{			
			if (!include.getIgnore())
			{
				bool bIgnore = false;
				{
					auto itIncludeToIgnore = std::find(a_includesToIgnore.begin(), a_includesToIgnore.end(), include);
					if (itIncludeToIgnore != a_includesToIgnore.end())
					{
						// store, for which compile file this include was ignored. This may be useful later.
						itIncludeToIgnore->addIgnoreCase(CFilePos(a_sCompileFile, include.getLine()));
						include.setToIgnore();
					}
				}
			}
		}
	}

	void filterIncludesWithCompileFileName(const std::string &a_sCompileFile, INCLUDES &a_includes)
	{
		const platform::string sCompileFileNamePure = STRING_TO_PLATFORM(a_sCompileFile).stem();

		// very likely a file which is named the same as the compile file should never be removed.
		for (auto &include : a_includes)
		{	
			if (!include.getIgnore())
			{
				const platform::string sIncludeFileNamePure = STRING_TO_PLATFORM(include.getFile()).stem();
				if (tools::strings::compareCaseInsensitive(sCompileFileNamePure, sIncludeFileNamePure))
					include.setToIgnore();
			}
		}
	}

	void filterIncludesByPreProcessResult(const platform::string &a_sPreProcessFile, INCLUDES &a_includes)
	{
		INCLUDE_HANDLES includesToRemove;
		{
			const std::string sPreProcessResult = tools::filesystem::readFile(a_sPreProcessFile);
			size_t iStart = 0;
			for (auto &include : a_includes)
			{
				if (!include.getIgnore())
				{
					const size_t iPos = sPreProcessResult.find(include.getMarkerVariableForPreProcess(), iStart);
					if (iPos != std::string::npos)
						iStart = iPos;
					else
						include.setToIgnore();
				}
			}
		}
	}	


}
