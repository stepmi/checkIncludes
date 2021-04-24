#pragma once
#include "compiler/ICompiler.h"

namespace compiler
{
	// g++
	class CGppCompiler : public compiler::ICompiler
	{
	public:
		
		ECompilerType getType() const override
		{
			return ECompilerType::eGpp;
		}

		compiler::EResult run(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, 
			const compiler::OPTIONS &a_options, platform::string &a_rsResultFile) const override;
		std::string getCompileFileFromCommandLine(const compileFile::COMMANDLINE &a_commandline) const override;
	};
}