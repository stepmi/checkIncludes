#pragma once

#include "projectFile/IProject.h"
#include "main/CParameters.h"
#include "compileFile/ICompileFile.h"

namespace preprocess
{

	platform::string getPreProcessResultPath(const projectFile::IProject &a_project, const CParameters &a_parameters, compileFile::ICompileFile &a_compileFile);

}