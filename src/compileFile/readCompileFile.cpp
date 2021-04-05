#include "readCompileFile.h"
#include "CCompileFile.h"
#include "tools/filesystem.h"
#include "parser/parser.h"

namespace compileFile
{
	std::unique_ptr<ICompileFile> readCompileFile(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy,
		const platform::string &a_wsProjectFile, const std::string &a_sStdAfx)
	{
		const platform::string wsFilePath = getCompileFilePath(a_sCompileFileWorkingCopy, a_wsProjectFile);
		const std::string sSrcCode = tools::filesystem::readFile(wsFilePath);
		if (!sSrcCode.empty())
		{
			const INCLUDES includes = parser::findIncludes(sSrcCode);					
			auto upResult = std::make_unique<CCompileFile>(a_sCompileFile, a_sCompileFileWorkingCopy, a_wsProjectFile, sSrcCode, includes);
			// If we have a valid a_sStdAfx we want to switch it off.
			// Assuming, that all files compile correct without including stdafx.
			// Use commandline option if this isn't the case. Then a_sStdAfx should be empty.
			if (!a_sStdAfx.empty())
				upResult->switchOffIncludeStdAfx(); 
			return upResult;
		}
		return std::unique_ptr<ICompileFile>(nullptr);
	}

	

}

