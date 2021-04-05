#include "compileFile/compileFile.h"
#include "compileFile/ICompileFile.h"
#include "parser/parser.h"
#include "compiler/ICompiler.h"
#include "tools/filesystem.h"
#include "system/logger.h"
#include "main/CParameters.h"
#include "tools/strings.h"
#include "readCompileFile.h"
#include "cloneProject.h"
#include "preProcess.h"
#include "projectFile/IThread.h"

namespace compileFile
{
	enum class ETestIncludeResult
	{
		eCompileOk,
		eCompileFailed,
		eFileSystemError, // reading/writing file
		eCompileError // compiler couldn't be started
	};

	compiler::OPTIONS getCompileOptions(const CParameters &a_parameters)
	{
		if (a_parameters.getHasOption(EOption::eCompileLog))
			return { compiler::EOption::eLogAll };
		return {};
	}

	ETestIncludeResult testInclude(const compiler::ICompiler &a_compiler, const CParameters &a_parameters, ICompileFile &a_compileFile, const HANDLE_INCLUDE &a_hInclude)
	{
		// disable the include
		const bool bSwitchIncludeOn = true;
		if (!a_compileFile.switchInclude(a_hInclude, !bSwitchIncludeOn))
			return ETestIncludeResult::eFileSystemError;

		logger::add(logger::EType::eDiagnose, "compiling " + tools::strings::getQuoted(a_compileFile.getFile()));

		const compiler::EResult eResult = a_compiler.run(a_compileFile, compiler::EAction::eCompile, a_parameters, getCompileOptions(a_parameters));
		if (eResult != compiler::EResult::eOk)
		{
			// reenable the include
			if (!a_compileFile.switchInclude(a_hInclude, bSwitchIncludeOn))
				return ETestIncludeResult::eFileSystemError;
			if (eResult == compiler::EResult::eFailed)
				return ETestIncludeResult::eCompileFailed;
			else
				return ETestIncludeResult::eCompileError;
		}
		return ETestIncludeResult::eCompileOk;
	}

	bool buildOriginalFile(const compiler::ICompiler &a_compiler, const CParameters &a_parameters, ICompileFile &a_compileFile)
	{		
		const compiler::EResult eResult = a_compiler.run(a_compileFile, compiler::EAction::eReBuild, a_parameters, getCompileOptions(a_parameters));
		if (eResult != compiler::EResult::eOk)
		{
			logger::add(logger::EType::eError, "Abort checking file " + tools::strings::getQuoted(a_compileFile.getFile()) + ". File doesn't compile at all. See errors below.");
			// we want to let the user know, why the file didn't compile
			if (!a_parameters.getHasOption(EOption::eCompileLog))
				a_compiler.run(a_compileFile, compiler::EAction::eReBuild, a_parameters, { compiler::EOption::eLogErrors });
			return false;
		}
		return true;
	}

	platform::string preProcess(projectFile::IProject &a_project, const compiler::ICompiler &a_compiler, const CParameters &a_parameters, ICompileFile &a_compileFile)
	{
		// now preprocess the file and filter the include files with the result.
		// we can ignore all includes that are not to be found in the preprocess result.
		// if this goes wrong somehow, we ignore that and just keep the includes as they are	
		platform::string sPreProcessResultFile;
		if (a_project.switchPreProcessOnly(true))
		{

			if (a_compileFile.addMarkersForPreProcess())
			{
				const compiler::EResult eResult = a_compiler.run(a_compileFile, compiler::EAction::eReBuild, a_parameters, getCompileOptions(a_parameters));
				if (eResult == compiler::EResult::eOk)
				{
					sPreProcessResultFile = preprocess::getPreProcessResultPath(a_project, a_parameters, a_compileFile);
				}
			}
			a_compileFile.removeMarkersForPreProcess();
		}
		a_project.switchPreProcessOnly(false);
		return sPreProcessResultFile;
	}
	
	void checkCompileFileIncludes(const compiler::ICompiler &a_compiler, const CParameters &a_parameters, ICompileFile &a_compileFile)
	{		
		std::vector<std::string> messages;

		auto includes = a_compileFile.getIncludes();

		// now try to disable includes
		for (auto &hInclude : includes)
		{
			const CInclude *pInclude = a_compileFile.getInclude(hInclude);
			if (pInclude)
			{
				const ETestIncludeResult eResult = testInclude(a_compiler, a_parameters, a_compileFile, hInclude);
				if (eResult == ETestIncludeResult::eFileSystemError)
				{
					logger::add(logger::EType::eError, "Abort checking file " + tools::strings::getQuoted(a_compileFile.getFile()) + ". Couldn't write to file");
					return;
				}
				else if (eResult == ETestIncludeResult::eCompileError)
				{
					logger::add(logger::EType::eError, "Abort checking file " + tools::strings::getQuoted(a_compileFile.getFile()) + ". Couldn't start compiler");
					return;
				}
				else if (eResult == ETestIncludeResult::eCompileOk)
				{
					messages.push_back(a_compileFile.getFile() + " line " + tools::strings::itos(pInclude->getLine()) + " " + pInclude->getTextForMessage() + " can be removed");
				}
			}
		}
		logger::add(logger::EType::eMessage, messages);
	}



	void checkCompileFile(projectFile::IThread &a_thread, const compiler::ICompiler &a_compiler, const CParameters &a_parameters, const std::string a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore)
	{
		auto upProject = cloneProject(a_parameters, a_sCompileFile);
		if (upProject)
		{
			auto upCompileFile = readCompileFile(a_sCompileFile, upProject->getCompileFileWorkingCopy(), upProject->getProjectFileWorkingCopy(), 
				a_parameters.getHasOption(EOption::eRequiresPrecompiledHeaders) ? std::string() : upProject->getStdAfx());
			if (upCompileFile)
			{
				if (!upCompileFile->getIncludes().empty())
				{
					auto sPreProcessResultFile = preProcess(*upProject, a_compiler, a_parameters, *upCompileFile);
					upCompileFile->filterIncludes(a_includesToIgnore, sPreProcessResultFile);
					if (!upCompileFile->getIncludes().empty())
					{
						if (buildOriginalFile(a_compiler, a_parameters, *upCompileFile))
						{						
							checkCompileFileIncludes(a_compiler, a_parameters, *upCompileFile);
						}
					}
				}
			}
			else
			{
				logger::add(logger::EType::eError, "Skipping file " + tools::strings::getQuoted(a_sCompileFile) + ". Couldn't read file");
			}
		}
		else
		{
			logger::add(logger::EType::eError, "Skipping file " + tools::strings::getQuoted(a_sCompileFile) + ". Couldn't clone project file.");
		}
		a_thread.setIsFinished();
	}


}