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
#include "compiler/createCompiler.h"

namespace compileFile
{
	static const bool bSwitchIncludeOn = true;

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
		if (!a_compileFile.switchInclude(a_hInclude, !bSwitchIncludeOn))
			return ETestIncludeResult::eFileSystemError;		

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

	const CInclude *getPossiblePreCompiledHeader(const ICompileFile &a_compileFile)
	{
		auto allIncludes = a_compileFile.getIncludes();
		if (!allIncludes.empty())
		{
			auto *pFirstInclude = a_compileFile.getInclude(allIncludes.front());
			if (pFirstInclude && !pFirstInclude->getIgnore())			
				return pFirstInclude;		
		}
		return nullptr;
	}

	bool buildOriginalFile(const projectFile::IProject &a_project, const compiler::ICompiler &a_compiler, const CParameters &a_parameters, ICompileFile &a_compileFile, std::vector<std::string> &a_rWarnings)
	{		
		// first we try to switch off the 1st include
		// very likely that is the precompiled header and the file should compile without it
		const CInclude *pPossiblePreCompiledHeader = nullptr;
		if (!a_project.getStdAfx().empty())
			pPossiblePreCompiledHeader = getPossiblePreCompiledHeader(a_compileFile);
		if (pPossiblePreCompiledHeader)
		{
			a_compileFile.switchInclude(pPossiblePreCompiledHeader->getHandle(), !bSwitchIncludeOn);
			const compiler::EResult eResult = a_compiler.run(a_compileFile, compiler::EAction::eReBuild, a_parameters, getCompileOptions(a_parameters));
			if (eResult == compiler::EResult::eOk)
			{
				// file compiles without the precompiled header.
				// we leave it in this state and return. This is our prefered case.
				return true;
			}

			// although the first include might be the precompiled header, it is very likely needed to compile the file.
			// So we switch it on again and try further.
			a_compileFile.switchInclude(pPossiblePreCompiledHeader->getHandle(), bSwitchIncludeOn);
		}

		const compiler::EResult eResult = a_compiler.run(a_compileFile, compiler::EAction::eReBuild, a_parameters, getCompileOptions(a_parameters));
		if (eResult != compiler::EResult::eOk)
		{
			// the file doesn't compile at all. That's an error.
			logger::add(logger::EType::eError, "Abort checking file " + tools::strings::getQuoted(a_compileFile.getFile()) + ". File doesn't compile at all. See errors below.");
			// we want to let the user know, why the file didn't compile
			if (!a_parameters.getHasOption(EOption::eCompileLog))
				a_compiler.run(a_compileFile, compiler::EAction::eReBuild, a_parameters, { compiler::EOption::eLogErrors });
			return false;		
		}

		//so the file did compile, but if there was a precompiled header, that would compromise the validity of our results
		if (pPossiblePreCompiledHeader)
		{
			a_rWarnings.emplace_back(a_compileFile.getFile() + " didn't compile without its precompiled header. The results may not be reliable at all.");
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
	
	void checkCompileFileIncludes(const compiler::ICompiler &a_compiler, const CParameters &a_parameters, ICompileFile &a_compileFile, std::vector<std::string> &a_rMessages, std::vector<std::string> &a_rCheckedIncludes)
	{				
		a_rCheckedIncludes.emplace_back("checked includes for " + a_compileFile.getFile() + ":");
		auto includes = a_compileFile.getIncludesToCheck();

		// now try to disable includes
		for (auto &hInclude : includes)
		{
			const CInclude *pInclude = a_compileFile.getInclude(hInclude);
			if (pInclude)
			{
				a_rCheckedIncludes.emplace_back("line " + tools::strings::itos(pInclude->getLine()) + " " + pInclude->getTextForMessage());
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
					a_rMessages.emplace_back(a_compileFile.getFile() + " line " + tools::strings::itos(pInclude->getLine()) + " " + pInclude->getTextForMessage() + " can be removed");
				}
			}
		}		
	}



	void checkCompileFile(projectFile::IThread &a_thread, const CParameters &a_parameters, const std::string a_sCompileFile, INCLUDES_TO_IGNORE &a_includesToIgnore)
	{
		auto upCompiler = compiler::createCompiler(ECompilerType::eMsVc); // TODO, we have to store a compiler type for each compile file
		if (upCompiler)
		{
			logger::add(logger::EType::eProcessedFiles, "Processing file " + tools::strings::getQuoted(a_sCompileFile));
			auto upProject = cloneProject(a_parameters, a_sCompileFile);
			if (upProject)
			{
				auto upCompileFile = readCompileFile(a_sCompileFile, upProject->getCompileFileWorkingCopy(), upProject->getProjectFileWorkingCopy());
				if (upCompileFile)
				{
					if (!upCompileFile->getIncludes().empty())
					{
						auto sPreProcessResultFile = preProcess(*upProject, *upCompiler, a_parameters, *upCompileFile);
						upCompileFile->filterIncludes(a_includesToIgnore, sPreProcessResultFile);
						if (!upCompileFile->getIncludesToCheck().empty())
						{
							std::vector<std::string> warnings, messages, checkedIncludes;
							if (buildOriginalFile(*upProject, *upCompiler, a_parameters, *upCompileFile, warnings))
							{
								checkCompileFileIncludes(*upCompiler, a_parameters, *upCompileFile, messages, checkedIncludes);
							}
							if (!messages.empty())
							{
								logger::add(logger::EType::eWarning, warnings);
								logger::add(logger::EType::eCheckedIncludes, checkedIncludes);
								logger::add(logger::EType::eMessage, messages);
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


}