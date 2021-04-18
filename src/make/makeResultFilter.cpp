#include "makeResultFilter.h"
#include "compiler/getCompilerType.h"

namespace make
{


	compileFile::COMPILE_FILES filterCommandLines(const COMMANDLINES &a_commandLines)
	{
		compileFile::COMPILE_FILES result;		
		for (const auto &commandLine : a_commandLines)
		{
			if (!commandLine.empty())
			{
				const auto eCompilerType = compiler::getCompilerTypeFromCommand(commandLine.front());
				if (eCompilerType != ECompilerType::eUnknown)
				{
					result.push_back(compileFile::CCompileFileInfo(eCompilerType, "", commandLine));
				}
			}
		}
		return result;
	}
}