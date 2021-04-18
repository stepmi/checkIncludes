#include "makeResultFilter.h"

namespace make
{
	compileFile::COMPILE_FILES filterCommandLines(const COMMANDLINES &a_commandLines)
	{
		compileFile::COMPILE_FILES result;		
		for (const auto &commandLine : a_commandLines)
		{
			// TODO
			result.push_back(compileFile::CCompileFileInfo(ECompilerType::eGcc, "", commandLine));
		}
		return result;
	}
}