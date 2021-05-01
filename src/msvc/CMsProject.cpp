#include "msvc/CMsProject.h"
#include "system/exit.h"
#include "tools/filesystem.h"
#include "msvc/CMsProjectFile.h"

namespace msvc
{

	CMsProject::CMsProject(const CParameters &a_parameters, const platform::string &a_wsProjectFile, const std::string &a_sCompileFile, 
		const std::string &a_sStdAfx, const platform::string &a_wsIntermediateDir) :
		m_parameters(a_parameters), m_wsProjectFile(a_wsProjectFile), m_sCompileFile(a_sCompileFile), 
		m_sStdAfx(a_sStdAfx), m_wsIntermediateDir(a_wsIntermediateDir),
		m_managedFileWorkingCopy(a_wsProjectFile),
		m_managedFileIntermediateDir(a_wsIntermediateDir)
	{		
	}	

	platform::string CMsProject::getProjectFileWorkingCopy() const
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

	bool CMsProject::switchPreProcessOnly(const bool a_bOn)
	{
		msvc::CMsProjectFile msProjectFile(m_parameters);

		if (msProjectFile.load(m_wsProjectFile))
		{
			if (msProjectFile.switchPreProcessOnly(a_bOn))
			{
				if (msProjectFile.save(m_wsProjectFile))
					return true;
			}
		}
		return false;
	}

}