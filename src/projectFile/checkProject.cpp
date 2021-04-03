#include "projectFile/projectFile.h"
#include "system/logger.h"
#include "tools/strings.h"
#include "threadpool.h"
#include "compileFile/CIncludeFileIgnore.h"
#include "main/CParameters.h"
#include "getCompileFiles.h"
#include "setConfiguration.h"

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

	compileFile::INCLUDES_TO_IGNORE getIncludesToIgnore(const std::vector<std::string> &a_sFiles)
	{
		compileFile::INCLUDES_TO_IGNORE result;
		for (auto &sFile : a_sFiles)
		{
			if (sFile.find(".h") != std::string::npos)
				result.emplace_back(compileFile::CIncludeToIgnore(sFile));
		}
		return result;
	}

	void printFiles(const std::string &a_sName, const std::vector<std::string> &a_files)
	{
		if (!a_files.empty())
		{
			std::string s;
			for (auto &sFile : a_files)
			{
				if (!s.empty())
					s += ", ";
				s += sFile;
			}
			logger::add(logger::EType::eMessage, a_sName + s);
		}
	}

	void printParameters(const CParameters &a_parameters)
	{
		if (a_parameters.getHasOption(EOption::eRequiresPrecompiledHeaders))
			logger::add(logger::EType::eMessage, "option -r is enabled. Results aren't reliable, if the header file is included by the precompiled header.");
		printFiles("files to compile: ", a_parameters.getCompileFiles());
		printFiles("files to ignore: ", a_parameters.getIgnoreFiles());
		logger::add(logger::EType::eMessage, "configuration: " + a_parameters.getProjectConfiguration().m_sPlatform + "/" + a_parameters.getProjectConfiguration().m_sConfiguration);
	}

	void checkProject(const compiler::ICompiler &a_compiler, const CParameters &a_parameters)
	{
		CParameters parameters = a_parameters;
		if (setConfiguration(parameters))
		{
			printParameters(parameters);
			std::vector<std::string> compileFiles = getCompileFiles(parameters);
			if (!compileFiles.empty())
			{
				compileFile::INCLUDES_TO_IGNORE includesToIgnore = getIncludesToIgnore(parameters.getIgnoreFiles());
				auto upThreadPool = threads::createThreadPool();
				if (upThreadPool)
				{
					//while (compileFiles.size() > 80)
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