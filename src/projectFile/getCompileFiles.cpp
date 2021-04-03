#include "getCompileFiles.h"
#include "system/logger.h"
#ifdef  _WIN32
	#include "msvc/CMsProjectFileQuery.h"
#endif


namespace projectFile
{

	void removeCompileFilesToIgnore(std::vector<std::string> &a_rCompileFiles, const std::vector<std::string> &a_filesToIgnore)
	{
		for (auto it = a_rCompileFiles.begin(); it != a_rCompileFiles.end(); )
		{
			auto &sCompileFile = *it;
			if (tools::find(a_filesToIgnore, sCompileFile))
				it = a_rCompileFiles.erase(it);
			else
				it++;
		}
	}

	void restrictCompileFiles(std::vector<std::string> &a_rCompileFiles, const std::vector<std::string> &a_filesToRestrictTo)
	{
		for (auto it = a_rCompileFiles.begin(); it != a_rCompileFiles.end(); )
		{
			auto &sCompileFile = *it;
			if (tools::find(a_filesToRestrictTo, sCompileFile))
				it++;
			else
				it = a_rCompileFiles.erase(it);
		}
	}

	std::vector<std::string> getCompileFiles(const CParameters &a_parameters)
	{
		std::vector<std::string> compileFiles;
		if (a_parameters.getCompilerType() == ECompilerType::eMsVc)
		{
#ifdef  _WIN32
			msvc::CMsProjectFileQuery msProjectFileQuery;
			if (!msProjectFileQuery.load(a_parameters.getProject()))
			{
				logger::add(logger::EType::eError, "Couldn't read " + PLATFORM_TOSTRING(a_parameters.getProject()));
				return {};
			}

			compileFiles = msProjectFileQuery.getCompileFiles();
			removeCompileFilesToIgnore(compileFiles, a_parameters.getIgnoreFiles());

			if (!a_parameters.getCompileFiles().empty())
			{
				restrictCompileFiles(compileFiles, a_parameters.getCompileFiles());
				// check, if we have found them all in the project file
				for (auto &sExpectedCompileFile : a_parameters.getCompileFiles())
				{
					if (!tools::find(compileFiles, sExpectedCompileFile))
					{
						logger::add(logger::EType::eError, sExpectedCompileFile + " not found in the project file.");
					}
				}
			}
#endif
		}

		return compileFiles;
	}

}