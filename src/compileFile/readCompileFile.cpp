#include "readCompileFile.h"
#include "CCompileFile.h"
#include "tools/filesystem.h"
#include "parser/parser.h"

namespace compileFile
{
	std::unique_ptr<ICompileFile> readCompileFile(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy, const platform::string &a_wsProjectFile)
	{
		const platform::string wsFilePath = getCompileFilePath(a_sCompileFileWorkingCopy, a_wsProjectFile);
		const std::string sSrcCode = tools::filesystem::readFile(wsFilePath);
		if (!sSrcCode.empty())
		{
			const INCLUDES includes = parser::findIncludes(sSrcCode);					
			return std::make_unique<CCompileFile>(a_sCompileFile, a_sCompileFileWorkingCopy, a_wsProjectFile, sSrcCode, includes);			
		}
		return std::unique_ptr<ICompileFile>(nullptr);
	}

	

}

