#pragma once

#include "projectFile/IProject.h"
#include "main/CParameters.h"

namespace make
{

	class CMakeProject : public projectFile::IProject
	{
	public:
		CMakeProject(const CParameters &a_parameters, const std::string &a_sCompileFile, const std::string &a_sStdAfx) :
			m_sStdAfx(a_sStdAfx), m_sCompileFile(a_sCompileFile), m_parameters(a_parameters)
		{}

		platform::string getProjectFileWorkingCopy() const override { return platform::string(); }
		std::string getStdAfx() const override { return m_sStdAfx; }
		const std::string &getCompileFileWorkingCopy() const override { return m_sCompileFile; }

		bool switchPreProcessOnly(const bool) override
		{
			return true;
		}

	private:
		std::string m_sStdAfx,
			m_sCompileFile; // working copy
		const CParameters m_parameters;
	};
}
