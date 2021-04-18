#pragma once

#include "compileFile/CIncludeFileIgnore.h"
#include "main/CParameters.h"
#include <memory>
#include "tools/interface.h"
#include "compileFile/CCompileFileInfo.h"

namespace threads
{	

	ci_interface IThreadPool
	{
		virtual ~IThreadPool() = default;
		
		virtual void addJob(const CParameters &a_parameters, const compileFile::CCompileFileInfo a_CompileFileInfo, compileFile::INCLUDES_TO_IGNORE &a_includesToIgnore) = 0;
		virtual void waitForAll() = 0;
	};

	std::unique_ptr<IThreadPool> createThreadPool();

}