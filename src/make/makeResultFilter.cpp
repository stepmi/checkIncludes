#include "makeResultFilter.h"
#include "compiler/getCompilerType.h"
#include "compiler/createCompiler.h"
#include "system/logger.h"
#include "system/execute.h"


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
					auto upCompiler = compiler::createCompiler(eCompilerType);
					if (upCompiler)
					{
						const auto sCompileFile = upCompiler->getCompileFileFromCommandLine(commandLine);
						if (!sCompileFile.empty())
							result.push_back(compileFile::CCompileFileInfo(eCompilerType, "", commandLine));
						else
							logger::add(logger::EType::eError, "Couldn't find compile file from command: " + execute::createCommandFromCommandLine(commandLine));
					}
				}
			}
		}
		return result;
	}
}