#include "filterIncludes.h"
#include "tools/strings.h"
#include <mutex>
#include <algorithm>

namespace compileFile
{
	std::mutex mutex_includesToIgnore;

	void filterIncludesByIncludesToIgnore(const std::string &a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore, INCLUDES &a_includes)
	{
		std::lock_guard<std::mutex> guard(mutex_includesToIgnore);
		for (auto it = a_includes.begin(); it != a_includes.end(); )
		{
			const CInclude &include = *it;
			bool bIgnore = false;
			{
				auto itIncludeToIgnore = std::find(a_includesToIgnore.begin(), a_includesToIgnore.end(), include);
				if (itIncludeToIgnore != a_includesToIgnore.end())
				{
					// store, for which compile file this include was ignored. This may be useful later.
					itIncludeToIgnore->addIgnoreCase(CFilePos(a_sCompileFile, include.getLine()));
					bIgnore = true;
				}
			}
			if (bIgnore)
				it = a_includes.erase(it);
			else
				it++;
		}
	}

	void filterIncludesWithCompileFileName(const std::string &a_sCompileFile, INCLUDES &a_includes)
	{
		const platform::string sCompileFileNamePure = STRING_TO_PLATFORM(a_sCompileFile).stem();

		// very likely a file which is named the same as the compile file should never be removed.
		for (auto it = a_includes.begin(); it != a_includes.end(); )
		{
			const CInclude &include = *it;

			const platform::string sIncludeFileNamePure = STRING_TO_PLATFORM(include.getFile()).stem();
			if (tools::strings::compareCaseInsensitive(sCompileFileNamePure, sIncludeFileNamePure))
				it = a_includes.erase(it);
			else
				it++;
		}
	}

	void filterIncludes(const std::string &a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore, INCLUDES &a_includes)
	{
		if (!a_includes.empty())
		{
			filterIncludesByIncludesToIgnore(a_sCompileFile, a_includesToIgnore, a_includes);

			// very likely a file which is named the same as the compile file should never be removed.
			const bool bIgnoreFilesWithCompileFileName = true;
			if (bIgnoreFilesWithCompileFileName)
			{
				filterIncludesWithCompileFileName(a_sCompileFile, a_includes);
			}
		}
	}



}
