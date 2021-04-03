#pragma once

#include "compileFile/CIncludeFileIgnore.h"
#include "main/CParameters.h"
#include <memory>
#include "tools/interface.h"

namespace compiler
{
	ci_interface ICompiler;
}

namespace threads
{	

	ci_interface IThreadPool
	{
		virtual ~IThreadPool() = default;
		
		virtual void addJob(const compiler::ICompiler &a_compiler, const CParameters &a_parameters, const std::string a_sCompileFile, compileFile::INCLUDES_TO_IGNORE &a_includesToIgnore) = 0;
		virtual void waitForAll() = 0;
	};

	std::unique_ptr<IThreadPool> createThreadPool();

}