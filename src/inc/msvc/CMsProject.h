#pragma once

#include "projectFile/IProject.h"

namespace msvc
{

	class CMsProject : public projectFile::IProject
	{
	public:
		CMsProject(const platform::string &a_wsProjectFile, const std::string &s_sCompileFile, const std::string &s_sStdAfx, const platform::string &a_wsIntermediateDir);
		~CMsProject();

		const platform::string &getProjectFileWorkingCopy() const override;
		std::string getStdAfx() const override;
		const std::string &getCompileFileWorkingCopy() const override;
		
	private:
		const platform::string m_wsProjectFile; // our working copy					
		const platform::string m_wsIntermediateDir;
		std::string m_sStdAfx,
			m_sCompileFile; // working copy
	};
}