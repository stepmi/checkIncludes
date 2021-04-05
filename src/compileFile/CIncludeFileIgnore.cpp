#include "compileFile/CIncludeFileIgnore.h"
#include "compileFile/CIncludeFile.h"
#include "tools/strings.h"

namespace compileFile
{

	CIncludeToIgnore::CIncludeToIgnore(const std::string &a_sInclude) :
		m_sInclude(a_sInclude)
	{}
	const FILEPOSITIONS CIncludeToIgnore::getIgnoreCases() const { return m_ignoreCases; }

	bool CIncludeToIgnore::operator==(const CInclude &a_include) const
	{
		return tools::strings::compareUserFileName(a_include.getFile(), m_sInclude);
	}

	void CIncludeToIgnore::addIgnoreCase(const CFilePos &a_filePos)
	{
		m_ignoreCases.push_back(a_filePos);
	}
	const std::string &CIncludeToIgnore::getInclude() const { return m_sInclude; }

}