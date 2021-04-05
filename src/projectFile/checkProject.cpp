#include "projectFile/projectFile.h"
#include "system/logger.h"
#include "tools/strings.h"
#include "threadpool.h"
#include "compileFile/CIncludeFileIgnore.h"
#include "main/CParameters.h"
#include "getCompileFiles.h"
#include "setConfiguration.h"
#include "tools/filename.h"

namespace projectFile
{	
	void checkIncludesToIgnoreUsed(const compileFile::INCLUDES_TO_IGNORE &a_includesToIgnore)
	{
		for (auto &includeToIgnore : a_includesToIgnore)
		{
			if (includeToIgnore.getIgnoreCases().empty())
			{
				// that is very likely a spelling error by the user, so we add a warning
				logger::add(logger::EType::eWarning, tools::strings::getQuoted(includeToIgnore.getInclude()) + " was set to ignore, but was never used in the files to compile");
			}
		}
	}

	bool checkPrintFilesMultiline(const std::vector<std::string> &a_files)
	{
		for (auto &sFile : a_files)
		{
			if (tools::filename::hasDirectory(sFile))
				return true;
			else if (sFile.size() > 30)
				return true;
		}
		return false;
	}
	
	void printFiles(const std::string &a_sName, const std::vector<std::string> &a_files)
	{
		if (!a_files.empty())
		{
			const bool bPrintMultiline = checkPrintFilesMultiline(a_files);
			std::string s;
			if (bPrintMultiline)
				logger::add(logger::EType::eMessage, a_sName);
			else
				s += a_sName + " ";
			for (auto &sFile : a_files)
			{
				if (bPrintMultiline)
					logger::add(logger::EType::eMessage, sFile);
				else
				{
					if (sFile != a_files.front())
						s += ", ";
					s += sFile;
				}
			}
			if (!bPrintMultiline)
				logger::add(logger::EType::eMessage, s);
		}
	}



	void printExtendedInfo(const CParameters &a_parameters, const std::vector<std::string> &a_compileFiles, const std::vector<std::string> &a_removedCompileFiles)
	{		
		if (!a_parameters.getCompileFiles().empty() || !a_parameters.getIgnoreCompileFiles().empty())
		{
			// the user provided options to restrict the compile files
			// so we we want to present the results of the restriction
			if (a_compileFiles.size() < a_removedCompileFiles.size())
				printFiles("files to compile:", a_compileFiles);
			else
				printFiles("compile files to ignore:", a_removedCompileFiles);
		}		
		// print includes to ignore
		printFiles("includes to ignore:", a_parameters.getIgnoreIncludes());
		logger::add(logger::EType::eMessage, "configuration: " + a_parameters.getProjectConfiguration().m_sPlatform + "/" + a_parameters.getProjectConfiguration().m_sConfiguration);
	}

	void checkProject(const compiler::ICompiler &a_compiler, const CParameters &a_parameters)
	{
		CParameters parameters = a_parameters;
		if (setConfiguration(parameters))
		{			
			std::vector<std::string> removedCompileFiles;
			std::vector<std::string> compileFiles = getCompileFiles(parameters, removedCompileFiles);
			if (!compileFiles.empty())
			{				
				compileFile::INCLUDES_TO_IGNORE includesToIgnore;				
				for (auto &sFile : parameters.getIgnoreIncludes())
					includesToIgnore.emplace_back(compileFile::CIncludeToIgnore(sFile));								
				printExtendedInfo(parameters, compileFiles, removedCompileFiles);
				auto upThreadPool = threads::createThreadPool();
				if (upThreadPool)
				{					
					while (!compileFiles.empty())
					{
						upThreadPool->addJob(a_compiler, parameters, compileFiles.front(), includesToIgnore);
						compileFiles.erase(compileFiles.begin());
					}
					upThreadPool->waitForAll();
				}
				checkIncludesToIgnoreUsed(includesToIgnore);
			}
			else
			{
				logger::add(logger::EType::eError, "No compile files found.");
			}
		}
	}

}