#include "cloneProject.h"
#include "system/logger.h"
#include "tools/strings.h"
#include "tools/filename.h"
#include "CCompileFile.h"
#include "tools/filesystem.h"
#include "system/exit.h"
#include "tools/find.h"
#include "CCompileFile.h"
#ifdef  _WIN32
	#include "msvc/CMsProjectFile.h"
	#include "msvc/CMsProject.h"
#endif

namespace compileFile
{

	platform::string getWorkingCopyProjectFile(const platform::string &a_wsPath, const std::string &a_sCompileFile)
	{
		platform::string wsWorkingCopyAdd = PLATFORM_T("_checkIncludes_");
		wsWorkingCopyAdd += STRING_TO_PLATFORM(tools::filename::getSpecificFileName(a_sCompileFile));

		platform::string wsWorkingCopy = a_wsPath.parent_path() / a_wsPath.stem();
		wsWorkingCopy += wsWorkingCopyAdd;
		wsWorkingCopy += a_wsPath.extension();
		return wsWorkingCopy;
	}

	std::string getCompileFileWorkingCopy(const std::string &a_sCompileFile)
	{
		std::string wsWorkingCopyAdd = "_checkIncludes";
		const platform::string wsCompileFile = STRING_TO_PLATFORM(a_sCompileFile);
		platform::string wsWorkingCopy = wsCompileFile.parent_path() / wsCompileFile.stem();
		wsWorkingCopy += wsWorkingCopyAdd;
		wsWorkingCopy += wsCompileFile.extension();
		return PLATFORM_TOSTRING(wsWorkingCopy);
	}

	bool cloneCompileFile(const platform::string &a_wsProject, const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy)
	{
		auto wsCompileFile = getCompileFilePath(a_sCompileFile, a_wsProject);
		auto wsCompileFileWorkingCopy = getCompileFilePath(a_sCompileFileWorkingCopy, a_wsProject);

		return tools::filesystem::copyFile(wsCompileFile, wsCompileFileWorkingCopy);
	}

	platform::string getIntermediateDirectoryPath(const std::string &a_sIntermediateDir, const platform::string &a_wsProjectFile)
	{
		return a_wsProjectFile.parent_path() / STRING_TO_PLATFORM(a_sIntermediateDir);
	}

	std::unique_ptr<projectFile::IProject> cloneProject(const CParameters &a_parameters, const std::string &a_sCompileFile)
	{
		if (a_parameters.getCompilerType() == ECompilerType::eMsVc)
		{
#ifdef  _WIN32
			msvc::CMsProjectFile msProjectFile(a_parameters);

			if (!msProjectFile.load(a_parameters.getProject()))
			{
				logger::add(logger::EType::eError, "Couldn't read " + PLATFORM_TOSTRING(a_parameters.getProject()));
				return std::unique_ptr<projectFile::IProject>(nullptr);
			}
			

			// getStdAfx now. It is removed by customize()
			const std::string sStdAfx = msProjectFile.getStdAfx();

			const std::string sWorkingCopyCompileFile = getCompileFileWorkingCopy(a_sCompileFile);
			if (!msProjectFile.customize(a_sCompileFile, sWorkingCopyCompileFile, a_parameters.getHasOption(EOption::eRequiresPrecompiledHeaders)))
			{
				logger::add(logger::EType::eError, "Couldn't read " + PLATFORM_TOSTRING(a_parameters.getProject()) + ". Error in file format.");
				return std::unique_ptr<projectFile::IProject>(nullptr);
			}

			const platform::string wsWorkingCopyProject = getWorkingCopyProjectFile(a_parameters.getProject(), a_sCompileFile);
			if (!msProjectFile.save(wsWorkingCopyProject))
			{
				logger::add(logger::EType::eError, "Couldn't create working copy project file for " + tools::strings::getQuoted(PLATFORM_TOSTRING(a_parameters.getProject())));
				return std::unique_ptr<projectFile::IProject>(nullptr);
			}

			if (!cloneCompileFile(a_parameters.getProject(), a_sCompileFile, sWorkingCopyCompileFile))
			{
				logger::add(logger::EType::eError, "Couldn't create working copy file for " + a_sCompileFile);
				return std::unique_ptr<projectFile::IProject>(nullptr);
			}

			const platform::string wsIntermediateDir = getIntermediateDirectoryPath(msvc::CMsProjectFile::getIntermediateDirectory(a_sCompileFile), a_parameters.getProject());

			return std::make_unique<msvc::CMsProject>(a_parameters, wsWorkingCopyProject, sWorkingCopyCompileFile, sStdAfx, wsIntermediateDir);
#endif
		}

		return std::unique_ptr<projectFile::IProject>(nullptr);		
	}



}