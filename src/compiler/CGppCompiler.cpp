#include "CGppCompiler.h"
#include <filesystem>
#include "tools/strings.h"
#include "compileFile/ICompileFile.h"
#include "tools/find.h"
#include "system/exit.h"
#include "tools/filesystem.h"
#include "tools/filename.h"
#include "system/execute.h"
#include "tools/CManagedFile.h"

namespace compiler
{

	bool getHasOption(const compiler::OPTIONS &a_options, const compiler::EOption a_eOption)
	{
		return tools::find(a_options, a_eOption);
	}


	class CTempFile
	{
	public:
		CTempFile(const std::string &a_sExtension) :
			m_managedFile(tools::filename::getTempFileName(a_sExtension))
		{}

		platform::string release()
		{
			return m_managedFile.release();
		}

		std::string getFileName() const
		{
			return m_managedFile.getFileName();
		}

	private:
		tools::CManagedFile m_managedFile;
	};


	bool getIsCompileFile(const std::string &a_sArgument)
	{
		const auto sExtension = std::filesystem::path(a_sArgument).extension().string();
		// see g++ man
		// we don't differentiate for c or c++
		const std::vector<std::string> extensions = { ".cc", ".cp", ".cxx", ".cpp", ".c++", "c" };
		for (auto &sExtensionCandidate : extensions)
		{			
			if (tools::strings::compareCaseInsensitive(sExtension, sExtensionCandidate))
				return true;
		}
		return false;
	}

	std::string CGppCompiler::getCompileFileFromCommandLine(const compileFile::COMMANDLINE &a_commandline) const
	{	
		// TODO: Is it possible, that there's more than one compile file, created with one command?
		// Then we'd have to split the calls up into one command for each compile file.
		for (const auto &sArgument : a_commandline)
		{
			if (&sArgument != &a_commandline.front())
			{
				if (getIsCompileFile(sArgument))
					return sArgument;
			}
		}
		return std::string();
	}
	
	
	void removeOption(const std::string &a_sOption, compileFile::COMMANDLINE &a_rCommandLine)
	{
		auto it = std::find(a_rCommandLine.begin(), a_rCommandLine.end(), a_sOption);
		if (it != a_rCommandLine.end())
			a_rCommandLine.erase(it);
	}
	
	void removeFileOption(const std::string &a_sOption, compileFile::COMMANDLINE &a_rCommandLine)
	{
		auto it = std::find(a_rCommandLine.begin(), a_rCommandLine.end(), a_sOption);
		if (it != a_rCommandLine.end())
		{			
			it = a_rCommandLine.erase(it);
			if (it != a_rCommandLine.end())
			{
				if (it->front() != '-')
					a_rCommandLine.erase(it);
			}
		}
	}	

	void addOptions(const std::vector<std::string> &a_options, compileFile::COMMANDLINE &a_rCommandLine)
	{
		// the first entry in a_rCommandLine is the command.
		// we add the new options just after that,
		// because order of options doesn't matter - but the compile files have to follow after all options.
		auto it = a_rCommandLine.begin();
		if (it != a_rCommandLine.end())
		{
			it++;
			for (auto itOption = a_options.rbegin(); itOption != a_options.rend(); itOption++)
			{
				it = a_rCommandLine.insert(it, *itOption);
			}
		}
	}

	void setOption(const std::string &a_sOption, compileFile::COMMANDLINE &a_rCommandLine)
	{
		if (!tools::find(a_rCommandLine, a_sOption))
			addOptions({ a_sOption }, a_rCommandLine);			
	}

	void setFileOption(const std::string &a_sOption, const std::string &a_sFile, compileFile::COMMANDLINE &a_rCommandLine)
	{
		removeFileOption(a_sOption, a_rCommandLine);
		addOptions({ a_sOption, a_sFile }, a_rCommandLine);
	}

	void setCompileFile(const platform::string &a_sCompileFile, compileFile::COMMANDLINE &a_rCommandLine)
	{
		a_rCommandLine.pop_back();
		a_rCommandLine.push_back(a_sCompileFile.string());
	}


	namespace options
	{
		// simple options
		static const std::string stopAfterCompile = "-c"; // Compile or assemble the source files, but do not link
		static const std::string stopAfterPreProcess = "-E"; // Stop after the preprocessing stage; do not run the compiler proper		
		static const std::string dependencies = "-MD"; // -MD is equivalent to -M -MF file, except that -E is not implied.		
		//static const std::string keepComments = "-C"; // Do not discard comments
		//static const std::string printCommands = "-v"; // Print (on standard error output) the commands executed to run the stages of compilation. 
		
		// file options
		static const std::string outputFile = "-o"; // -o file. Place the primary output in file file
		static const std::string targetFile = "-MT";	// -MT target. Change the target of the rule emitted by dependency generation
		static const std::string dependenciesFile = "-MF";	// -MF file. When used with -M or-MM, specifies a file to write the dependencies to
	}

	CResult CGppCompiler::run(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, 
		const compiler::OPTIONS &a_options) const
	{
		CResult result;
		auto commandline = a_compileFile.getCommandLine();
		if (commandline.size() > 1)
		{
			std::string sOutputExtension;
			if (a_eAction == compiler::EAction::ePreCompile)
			{
				sOutputExtension = ".d";
				removeOption(options::stopAfterCompile, commandline);
				setOption(options::stopAfterPreProcess, commandline);
				setOption(options::dependencies, commandline);				
			}
			else if (a_eAction == compiler::EAction::eCompile ||
				a_eAction == compiler::EAction::eReBuild)
			{
				sOutputExtension = ".o";
				setOption(options::stopAfterCompile, commandline);
			}
			// 2 different output files.
			// dependenciesFile is always discarded after build.
			// outputFile is kept for a preprocess build (because it contains the preprocess result),
			// and discarded for all other builds.

			CTempFile outputFile(sOutputExtension);
			setFileOption(options::outputFile, outputFile.getFileName(), commandline);
			setFileOption(options::targetFile, outputFile.getFileName(), commandline);

			CTempFile dependenciesFile(".d");
			setFileOption(options::dependenciesFile, dependenciesFile.getFileName(), commandline);

			setCompileFile(a_compileFile.getFileWorkingCopy(), commandline);

			const auto sCommandLine = execute::createCommandFromCommandLine(commandline);			

			execute::EResult eResult = execute::EResult::eError;
			if (getHasOption(a_options, compiler::EOption::eLogErrors))
				eResult = execute::runOutputToConsole(sCommandLine, platform::string());
			else							
				eResult = execute::runQuiet(sCommandLine, platform::string());			
			
			if (eResult == execute::EResult::eOk)
			{
				if (a_eAction == compiler::EAction::ePreCompile)
					result.upResultFile = std::make_unique<tools::CManagedFile>(outputFile.release());
				result.eResult = compiler::EResult::eOk;
			}
			else if (eResult == execute::EResult::eFailed)
			{				
				result.eResult = compiler::EResult::eFailed; // compile failed
			}
			else
				result.eResult = compiler::EResult::eError; // compile couldn't be started at all
		}
		result.eResult = compiler::EResult::eError; // compile couldn't be started at all

		return result;
	}

}