#pragma once

#include <memory>
#include <set>
#include "tools/interface.h"
#include "main/ECompilerType.h"
#include "compileFile/CCompileFileInfo.h"
#include "tools/platforms.h"

class CParameters;

namespace compileFile
{
	ci_interface ICompileFile;
}

namespace compiler
{
	enum class EAction 
	{
		eReBuild, // = clean & compile
		eCompile,
		ePreCompile
	};

	enum class EOption
	{
		eLogAll,
		eLogErrors
	};

	using OPTIONS = std::set<EOption>;

	enum class EResult 
	{ 
		eOk, 
		eFailed,	// compile finished with some error
		eError		// compile couldn't even be started
	};

	ci_interface ICompiler
	{
		virtual ~ICompiler() = default;		
		// a_rsResultFile is used for preprocess output only
		virtual EResult run(const compileFile::ICompileFile &a_compileFile, const EAction a_eAction, const CParameters &a_parameters, 
			const OPTIONS &a_options, platform::string &a_rsResultFile) const = 0;
		virtual ECompilerType getType() const = 0;
		virtual std::string getCompileFileFromCommandLine(const compileFile::COMMANDLINE &a_commandLine) const = 0;
	};		

}