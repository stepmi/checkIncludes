#include "CCompileFile.h"
#include "tools/filesystem.h"
#include "parser/parser.h"
#include "system/exit.h"
#include "system/logger.h"
#include "system/exit.h"

namespace compileFile
{

	CCompileFile::CCompileFile(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy,
		const platform::string &a_wsProjectFile,
		const std::string &a_sSrcCode, INCLUDES &a_includes) :
		m_sCompileFile(a_sCompileFile), m_sCompileFileWorkingCopy(a_sCompileFileWorkingCopy),
		m_wsProjectFile(a_wsProjectFile),
		m_sSrcCode(a_sSrcCode), m_includes(a_includes),
		m_iLenDisableInclude(static_cast<int>(m_sDisableInclude.size()))
	{
		exitHandler::add(getFilePath());
	}

	CCompileFile::~CCompileFile()
	{
		tools::filesystem::removeAll(getFilePath());
		exitHandler::remove(getFilePath());
	}
	
	INCLUDE_HANDLES CCompileFile::getIncludes() const
	{
		INCLUDE_HANDLES result;
		for (auto &include : m_includes)
			result.push_back(include.getHandle());
		return result;
	}

	const CInclude *CCompileFile::getInclude(const HANDLE_INCLUDE a_hInclude) const
	{
		for (auto &include : m_includes)
		{
			if (include.getHandle() == a_hInclude)
				return &include;
		}
		return nullptr;
	}

	bool CCompileFile::switchInclude(const HANDLE_INCLUDE a_hInclude, const bool a_bSwitchOn)
	{
		const int iOffset = a_bSwitchOn ? -m_iLenDisableInclude : m_iLenDisableInclude;
		bool bIncludeSwitched = false;
		for (auto &include : m_includes)
		{
			if (!bIncludeSwitched)
			{
				if (include.getHandle() == a_hInclude)
				{
					if (switchIncludeInSrcAndFile(include, a_bSwitchOn))
						bIncludeSwitched = true;
				}
			}
			else
			{
				include.offset(iOffset);
			}
		}
		return bIncludeSwitched;
	}

	void CCompileFile::switchOffIncludeStdAfx()
	{
		if (!m_includes.empty())
		{
			// stdafx is always first
			switchInclude(m_includes.front().getHandle(), false);
			m_includes.erase(m_includes.begin()); 
		}
	}

	platform::string CCompileFile::getFilePath() const
	{
		return getCompileFilePath(m_sCompileFileWorkingCopy, m_wsProjectFile);
	}

	bool CCompileFile::switchIncludeInSrcAndFile(const CInclude &a_include, const bool a_bSwitchOn)
	{
		if (a_bSwitchOn)
			m_sSrcCode.erase(a_include.getPos(), static_cast<size_t>(m_iLenDisableInclude));
		else
			m_sSrcCode.insert(a_include.getPos(), m_sDisableInclude);
		return tools::filesystem::writeFile(getFilePath(), m_sSrcCode);
	}

	platform::string getCompileFilePath(const std::string &a_sCompileFile, const platform::string &a_wsProjectFile)
	{
		return a_wsProjectFile.parent_path() / STRING_TO_PLATFORM(a_sCompileFile);
	}

}

