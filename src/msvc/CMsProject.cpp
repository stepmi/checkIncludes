#include "msvc/CMsProject.h"
#include "system/exit.h"
#include "tools/filesystem.h"

namespace msvc
{

	CMsProject::CMsProject(const platform::string &a_wsProjectFile, const std::string &s_sCompileFile, const std::string &s_sStdAfx, const platform::string &a_wsIntermediateDir) :
		m_wsProjectFile(a_wsProjectFile), m_sCompileFile(s_sCompileFile), m_sStdAfx(s_sStdAfx), m_wsIntermediateDir(a_wsIntermediateDir)
	{
		exitHandler::add(m_wsProjectFile);
		exitHandler::add(m_wsIntermediateDir);
	}

	CMsProject::~CMsProject()
	{
		// remove the working copy and the intermediate dir
		tools::filesystem::removeAll(m_wsProjectFile);
		exitHandler::remove(m_wsProjectFile);
		tools::filesystem::removeAll(m_wsIntermediateDir);
		exitHandler::remove(m_wsIntermediateDir);
	}


	const platform::string &CMsProject::getProjectFileWorkingCopy() const
	{
		return m_wsProjectFile;
	}

	std::string CMsProject::getStdAfx() const
	{
		return m_sStdAfx;
	}

	const std::string &CMsProject::getCompileFileWorkingCopy() const
	{
		return m_sCompileFile;
	}

}