#include "compiler/createCompiler.h"
#include "CGppCompiler.h"
#ifdef  _WIN32
	#include "msvc/CMsCompiler.h"
#endif

namespace compiler
{

	std::unique_ptr<ICompiler> createCompiler(const ECompilerType a_eType)
	{
		if (a_eType == ECompilerType::eMsVc)
		{
#ifdef  _WIN32
			return msvc::createMsCompiler();
#endif
		}
		else if (a_eType == ECompilerType::eGpp)
		{
			return std::make_unique<CGppCompiler>();
		}
		
		return std::unique_ptr<ICompiler>(nullptr);
	}

}