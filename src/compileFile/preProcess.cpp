#include "preProcess.h"
#ifdef _WIN32
#include "msvc/CMsProjectFile.h"
#endif

namespace preprocess
{

	platform::string getPreProcessResultPath(const projectFile::IProject &a_project, const CParameters &a_parameters, compileFile::ICompileFile &a_compileFile)
	{
		if (a_parameters.getCompilerType() == ECompilerType::eMsVc)
		{
#ifdef _WIN32
			auto sCompileFileRelative = STRING_TO_PLATFORM(a_compileFile.getFileWorkingCopy());			
			auto sPath = a_project.getProjectFileWorkingCopy().parent_path() / STRING_TO_PLATFORM(msvc::CMsProjectFile::getIntermediateDirectory(a_compileFile.getFile()));
			sPath /= sCompileFileRelative.filename();
			sPath.replace_extension("i");
			return sPath;
#endif
		}
		return platform::string();
	}

}