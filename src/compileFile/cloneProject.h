#pragma once

#include <memory>
#include <string>
#include "main/CParameters.h"
#include "projectFile/IProject.h"

namespace compileFile
{
	std::unique_ptr<projectFile::IProject> cloneProject(const CParameters &a_parameters, const std::string &a_sCompileFile);
}