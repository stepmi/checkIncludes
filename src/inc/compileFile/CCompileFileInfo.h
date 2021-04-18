#pragma once

#include <string>
#include <vector>
#include "main/ECompilerType.h"

namespace compileFile
{
	using COMMANDLINE = std::vector<std::string>; // this always a command and its arguments

	// a file from the project, that can be compiled. Most likely a .cpp or .c file.
	// this contains all the info, that's needed to start the check of a compile file
	class CCompileFileInfo
	{
	public:
		CCompileFileInfo(const ECompilerType a_eType, const std::string &a_sCompileFile, const COMMANDLINE &a_commandLine) :
			m_eType(a_eType), m_sCompileFile(a_sCompileFile), m_commandLine(a_commandLine)
		{}

		// queries
		const std::string &getCompileFile() const { return m_sCompileFile; }
		ECompilerType getCompilerType() const { return m_eType; }
		const COMMANDLINE &getCommandLine() const { return m_commandLine; }

	private:
		ECompilerType m_eType = ECompilerType::eUnknown;
		std::string m_sCompileFile;
		COMMANDLINE m_commandLine; // compiler call and its arguments. may be empty for some compiler types.	
	};

	using COMPILE_FILES = std::vector<CCompileFileInfo>;
}
