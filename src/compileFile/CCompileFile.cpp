#include "CCompileFile.h"
#include "tools/filesystem.h"
#include "parser/parser.h"
#include "system/exit.h"
#include "system/logger.h"
#include "system/exit.h"
#include "tools/find.h"
#include "filterIncludes.h"

namespace compileFile
{

	platform::string getCompileFilePath(const std::string &a_sCompileFile, const platform::string &a_wsProjectFile)
	{
		return a_wsProjectFile.parent_path() / STRING_TO_PLATFORM(a_sCompileFile);
	}

	CCompileFile::CCompileFile(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy,
		const platform::string &a_wsProjectFile,
		const std::string &a_sSrcCode, const INCLUDES &a_includes) :
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
	
	INCLUDE_HANDLES CCompileFile::getIncludesToCheck() const
	{
		INCLUDE_HANDLES result;
		for (auto &include : m_includes)
		{
			if (!include.getIgnore() && include.getEnabled())
				result.push_back(include.getHandle());
		}
		return result;
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

	CInclude *CCompileFile::getInclude(const HANDLE_INCLUDE a_hInclude)
	{
		return const_cast<CInclude*>(static_cast<const CCompileFile&>(*this).getInclude(a_hInclude));
	}

	bool CCompileFile::switchInclude(const HANDLE_INCLUDE a_hInclude, const bool a_bSwitchOn)
	{			
		CInclude *pInclude = getInclude(a_hInclude);
		if (pInclude)
		{
			if (switchIncludeInSrcAndFile(*pInclude, a_bSwitchOn))
			{
				const int iOffset = a_bSwitchOn ? -m_iLenDisableInclude : m_iLenDisableInclude;
				offsetIncludesAfter(a_hInclude, iOffset);
				return true;
			}
		}
		return false;
	}

	bool CCompileFile::addMarkersForPreProcess()
	{
		return switchMarkersForPreProcess(true);
	}

	bool CCompileFile::removeMarkersForPreProcess()
	{
		return switchMarkersForPreProcess(false);
	}
	
	bool CCompileFile::switchMarkersForPreProcess(const bool a_bSwitchOn)
	{
		for (auto &include : m_includes)
			switchMarkerForPreProcessInSrc(include, a_bSwitchOn);
		return tools::filesystem::writeFile(getFilePath(), m_sSrcCode);
	}

	/*void CCompileFile::switchOffIncludeStdAfx()
	{
		if (!m_includes.empty())
		{
			// stdafx is always first
			switchInclude(m_includes.front().getHandle(), false);
			m_includes.erase(m_includes.begin()); 
		}
	}*/

	platform::string CCompileFile::getFilePath() const
	{
		return getCompileFilePath(m_sCompileFileWorkingCopy, m_wsProjectFile);
	}

	bool CCompileFile::switchIncludeInSrcAndFile(CInclude &a_include, const bool a_bSwitchOn)
	{
		if (a_bSwitchOn)
			m_sSrcCode.erase(a_include.getPos(), static_cast<size_t>(m_iLenDisableInclude));
		else
			m_sSrcCode.insert(a_include.getPos(), m_sDisableInclude);
		a_include.setIsEnabled(a_bSwitchOn);
		return tools::filesystem::writeFile(getFilePath(), m_sSrcCode);
	}

	void CCompileFile::switchMarkerForPreProcessInSrc(const CInclude &a_include, const bool a_bSwitchOn)
	{
		const std::string sMarkerLine = a_include.getMarkerLineForPreProcess();
		if (a_bSwitchOn)
			m_sSrcCode.insert(a_include.getPos(), sMarkerLine);
		else					
			m_sSrcCode.erase(a_include.getPos(), static_cast<size_t>(sMarkerLine.size()));
			
		const int iMarkerLineLength = static_cast<int>(sMarkerLine.size());
		const int iOffset = a_bSwitchOn ? iMarkerLineLength : -iMarkerLineLength;
		offsetIncludesAfter(a_include.getHandle(), iOffset);		
	}

	void CCompileFile::offsetIncludesAfter(const HANDLE_INCLUDE a_hInclude, const int a_iOffset)
	{
		bool bStartOffsetting = false;
		for (auto &include : m_includes)
		{
			if (!bStartOffsetting)
			{
				if (include.getHandle() == a_hInclude)
					bStartOffsetting = true;
				
			}
			else			
				include.offset(a_iOffset);			
		}
	}	

	// we don't remove any includes. we just set them to ignore. 
	void CCompileFile::filterIncludes(INCLUDES_TO_IGNORE &a_includesToIgnore, const platform::string &a_sPreProcessFile)
	{	
		filterIncludesByIncludesToIgnore(m_sCompileFile, a_includesToIgnore, m_includes);

		// very likely a file which is named the same as the compile file should never be removed.
		filterIncludesWithCompileFileName(m_sCompileFile, m_includes);

		filterIncludesByPreProcessResult(a_sPreProcessFile, m_includes);
	}
	

}

