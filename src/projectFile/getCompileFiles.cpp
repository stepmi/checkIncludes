#include "getCompileFiles.h"
#include "system/logger.h"
#include "tools/strings.h"
#ifdef  _WIN32
	#include "msvc/CMsProjectFileQuery.h"
#endif


namespace projectFile
{

	bool findFileNameProvidedByUserInCompileFiles(const std::vector<std::string> &a_compileFiles, const std::string &a_sFileProvidedByUser)
	{
		for (const auto &sCompileFile : a_compileFiles)
		{
			if (tools::strings::compareUserFileName(sCompileFile, a_sFileProvidedByUser))
				return true;
		}
		return false;
	}

	bool findCompileFileInFilesProvidedByUser(const std::vector<std::string> &a_filesProvidedByUser, const std::string &a_sCompileFile)
	{
		for (const auto &sFileProvidedByUser : a_filesProvidedByUser)
		{
			if (tools::strings::compareUserFileName(a_sCompileFile, sFileProvidedByUser))
				return true;
		}
		return false;
	}

	// returns the removed files
	void removeCompileFilesToIgnore(std::vector<std::string> &a_rCompileFiles, const std::vector<std::string> &a_filesToIgnore, std::vector<std::string> &a_rRemovedCompileFiles)
	{		
		for (auto it = a_rCompileFiles.begin(); it != a_rCompileFiles.end(); )
		{
			auto &sCompileFile = *it;
			if (findCompileFileInFilesProvidedByUser(a_filesToIgnore, sCompileFile))
			{
				a_rRemovedCompileFiles.push_back(*it);
				it = a_rCompileFiles.erase(it);
			}
			else
				it++;
		}	
	}

	void restrictCompileFiles(std::vector<std::string> &a_rCompileFiles, const std::vector<std::string> &a_filesToRestrictTo, std::vector<std::string> &a_rRemovedCompileFiles)
	{
		for (auto it = a_rCompileFiles.begin(); it != a_rCompileFiles.end(); )
		{
			auto &sCompileFile = *it;
			if (findCompileFileInFilesProvidedByUser(a_filesToRestrictTo, sCompileFile))
				it++;
			else
			{
				a_rRemovedCompileFiles.push_back(*it);
				it = a_rCompileFiles.erase(it);
			}
		}
	}

	std::vector<std::string> getCompileFiles(const CParameters &a_parameters, std::vector<std::string> &a_rRemovedCompileFiles)
	{
		std::vector<std::string> compileFiles;
		if (a_parameters.getProjectType() == EProjectType::eMsBuild)
		{
#ifdef  _WIN32
			msvc::CMsProjectFileQuery msProjectFileQuery;
			if (!msProjectFileQuery.load(a_parameters.getProject()))
			{
				logger::add(logger::EType::eError, "Couldn't read " + PLATFORM_TOSTRING(a_parameters.getProject()));
				return {};
			}

			compileFiles = msProjectFileQuery.getCompileFiles();
#endif
		}


		removeCompileFilesToIgnore(compileFiles, a_parameters.getIgnoreCompileFiles(), a_rRemovedCompileFiles);

		if (!a_parameters.getCompileFiles().empty())
		{
			restrictCompileFiles(compileFiles, a_parameters.getCompileFiles(), a_rRemovedCompileFiles);
			// check, if we have found them all in the project file
			for (auto &sExpectedCompileFile : a_parameters.getCompileFiles())
			{
				if (!findFileNameProvidedByUserInCompileFiles(compileFiles, sExpectedCompileFile))
				{
					logger::add(logger::EType::eError, sExpectedCompileFile + " not found in the project file.");
				}
			}
		}
		return compileFiles;
	}

}