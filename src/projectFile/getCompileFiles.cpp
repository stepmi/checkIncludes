#include "getCompileFiles.h"
#include "system/logger.h"
#include "tools/strings.h"
#include "make/CMakeFile.h"
#ifdef  _WIN32
	#include "msvc/CMsProjectFileQuery.h"
#endif


namespace projectFile
{

	bool findFileNameProvidedByUserInCompileFiles(const compileFile::COMPILE_FILES &a_compileFiles, const std::string &a_sFileProvidedByUser)
	{
		for (const auto &compileFile : a_compileFiles)
		{
			if (tools::strings::compareUserFileName(compileFile.getCompileFile(), a_sFileProvidedByUser))
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
	void removeCompileFilesToIgnore(compileFile::COMPILE_FILES &a_rCompileFiles, const std::vector<std::string> &a_filesToIgnore, std::vector<std::string> &a_rRemovedCompileFiles)
	{		
		for (auto it = a_rCompileFiles.begin(); it != a_rCompileFiles.end(); )
		{
			const auto &sCompileFile = it->getCompileFile();
			if (findCompileFileInFilesProvidedByUser(a_filesToIgnore, sCompileFile))
			{
				a_rRemovedCompileFiles.push_back(sCompileFile);
				it = a_rCompileFiles.erase(it);
			}
			else
				it++;
		}	
	}

	void restrictCompileFiles(compileFile::COMPILE_FILES &a_rCompileFiles, const std::vector<std::string> &a_filesToRestrictTo, std::vector<std::string> &a_rRemovedCompileFiles)
	{
		for (auto it = a_rCompileFiles.begin(); it != a_rCompileFiles.end(); )
		{
			const auto &sCompileFile = it->getCompileFile();
			if (findCompileFileInFilesProvidedByUser(a_filesToRestrictTo, sCompileFile))
				it++;
			else
			{
				a_rRemovedCompileFiles.push_back(sCompileFile);
				it = a_rCompileFiles.erase(it);
			}
		}
	}

	compileFile::COMPILE_FILES getCompileFiles(const CParameters &a_parameters, std::vector<std::string> &a_rRemovedCompileFiles)
	{
		compileFile::COMPILE_FILES compileFiles;
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
		else if (a_parameters.getProjectType() == EProjectType::eMakeFile)
		{
			compileFiles = make::getCompileFiles(a_parameters.getProject().string());
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