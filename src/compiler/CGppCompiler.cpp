#include "CGppCompiler.h"
#include <filesystem>
#include "tools/strings.h"
#include "compileFile/ICompileFile.h"
#include "tools/find.h"
#include "system/exit.h"
#include "tools/filesystem.h"
#include "tools/filename.h"
#include "system/execute.h"

namespace compiler
{

	class CTempFile
	{
	public:
		CTempFile(const std::string &a_sExtension) :
			m_sFileName(tools::filename::getTempFileName(a_sExtension))
		{
			exitHandler::add(m_sFileName);
		}

		~CTempFile()
		{			
			if (!m_sFileName.empty())
			{
				tools::filesystem::removeAll(m_sFileName);
				exitHandler::remove(m_sFileName);
			}
		}

		platform::string release()
		{
			platform::string sResult = m_sFileName;
			exitHandler::remove(m_sFileName);
			m_sFileName.clear();
			return sResult;
		}

		std::string getFileName() const
		{
			return m_sFileName.string();
		}
		
		std::string readFile() const
		{
			return tools::filesystem::readFile(m_sFileName);
		}

	private:		
		platform::string m_sFileName;
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
		static const std::string keepComments = "-C"; // Do not discard comments

		// file options
		static const std::string output = "-o"; // -o file. Place the primary output in file file
		static const std::string targetFile = "-MT";	// -MT target. Change the target of the rule emitted by dependency generation
		static const std::string dependencies = "-MF";	// -MF file. When used with -M or-MM, specifies a file to write the dependencies to
	}

	compiler::EResult CGppCompiler::run(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, 
		const compiler::OPTIONS &a_options, platform::string &a_rsResultFile) const
	{
		auto commandline = a_compileFile.getCommandLine();
		if (commandline.size() > 1)
		{
			removeFileOption(options::targetFile, commandline);

			std::string sOutputExtension;
			if (a_eAction == compiler::EAction::ePreCompile)
			{
				sOutputExtension = ".d";
				removeOption(options::stopAfterCompile, commandline);
				setOption(options::stopAfterPreProcess, commandline);
				// keeping comments is important, because we added comments to filter our includes.
				setOption(options::keepComments, commandline);
			}
			else if (a_eAction == compiler::EAction::eCompile ||
				a_eAction == compiler::EAction::eReBuild)
			{
				sOutputExtension = ".o";
				setOption(options::stopAfterCompile, commandline);
			}

			CTempFile outputFile(sOutputExtension);
			setFileOption(options::output, outputFile.getFileName(), commandline);

			CTempFile dependenciesFile(".d");
			setFileOption(options::dependencies, dependenciesFile.getFileName(), commandline);

			setCompileFile(a_compileFile.getFileWorkingCopy(), commandline);

			const auto sCommandLine = execute::createCommandFromCommandLine(commandline);			
			auto eResult = execute::runOutputToConsole(sCommandLine, platform::string());
			if (eResult == execute::EResult::eOk)
			{
				if (a_eAction == compiler::EAction::ePreCompile)
					a_rsResultFile = outputFile.release();
				return compiler::EResult::eOk;
			}
			else if (eResult == execute::EResult::eFailed)
				return compiler::EResult::eFailed; // compile failed

			return compiler::EResult::eError; // compile couldn't be started at all
		}
		return compiler::EResult::eError; // compile couldn't be started at all
	}

}