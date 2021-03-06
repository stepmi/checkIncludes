#include "compileFile/CIncludeFile.h"
#include "tools/filesystem.h"
#include "tools/strings.h"

namespace compileFile
{
	CInclude::CInclude(const HANDLE_INCLUDE a_iHandle, const std::string &a_sFile, const size_t a_iPosStart, /*const size_t a_iCount,*/ const size_t a_iLine, const bool a_bIsSystemFile) :
		m_iHandle(a_iHandle), m_sFile(a_sFile), m_iPosStart(a_iPosStart), /*m_iCount(a_iCount),*/ m_iLine(a_iLine), m_bIsSystemFile(a_bIsSystemFile)
	{}

	HANDLE_INCLUDE CInclude::getHandle() const { return m_iHandle; }
	size_t CInclude::getPos() const { return m_iPosStart; }
	size_t CInclude::getLine() const { return m_iLine; }
	std::string CInclude::getFile() const { return m_sFile; }
	bool CInclude::getIgnore() const { return m_bIgnore; }
	void CInclude::setToIgnore() { m_bIgnore = true; }
	bool CInclude::getEnabled() const { return m_bIsEnabled; }
	void CInclude::setIsEnabled(const bool a_bValue) { m_bIsEnabled = a_bValue; }


	void CInclude::offset(const int a_iValue)
	{
		m_iPosStart += a_iValue;
	}

	std::string CInclude::getTextForMessage() const
	{
		const std::string s = "#include ";
		const std::string sQuote = "\"";
		if (m_bIsSystemFile)
			return s + "<" + m_sFile + ">";
		else
			return s + sQuote + m_sFile + sQuote;
	}

	std::string CInclude::getMarkerVariableForPreProcess() const
	{
		return "global_marker_checkIncludes_" + tools::strings::itos(m_iHandle);
	}

	std::string CInclude::getMarkerLineForPreProcess() const
	{
		return "static const int " + getMarkerVariableForPreProcess() + " = 0;\n";
	}	

}