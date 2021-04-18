#include "state.h"

namespace parser
{

	CState::CState(const std::string &a_sText) :
		m_sText(a_sText), m_iSize(a_sText.size())
	{}

	const std::string &CState::getText() const { return m_sText; }
	size_t CState::getPos() const { return m_iPos; }
	size_t CState::getLine() const { return m_iLine; }
	bool CState::getIsFinished() const { return m_iPos >= m_iSize; }

	bool CState::getIsInAnyComment() const
	{
		return m_bInSingleLineComment || m_bInMultiLineComment;
	}
	//bool CState::getIsInSingleLineComment() const { return m_bInSingleLineComment; }
	bool CState::getIsInMultiLineComment() const { return m_bInMultiLineComment; }
	bool CState::getIsInInclude() const { return m_bInInclude; }
	bool CState::getIsInPreProcess() const { return  m_iPosPreProcess != std::string::npos;; }
	bool CState::getIsInFileQuote() const { return m_iPosFileQuote != std::string::npos; }
	bool CState::getIsInSystemFileQuote() const { return m_iPosSystemFileQuote != std::string::npos; }
	bool CState::getIsInAnyQuote() const 
	{ 
		return m_iPosFileQuote != std::string::npos || m_iPosSystemFileQuote != std::string::npos;
	}

	size_t CState::getPosPreProcess() const { return m_iPosPreProcess; }
	size_t CState::getPosFileQuote() const { return m_iPosFileQuote; }
	size_t CState::getPosSystemFileQuote() const { return m_iPosSystemFileQuote; }

	const compileFile::INCLUDES &CState::getIncludes() const { return m_includes; }

	// manipulators
	void CState::incPos(const size_t a_iValue) 
	{
		m_iPos += a_iValue;
	}

	void CState::setFileQuote(const size_t a_iPos) { m_iPosFileQuote = a_iPos; }
	void CState::setSystemFileQuote(const size_t a_iPos) { m_iPosSystemFileQuote = a_iPos; }	
	void CState::setSingleLineComment(const bool a_bValue) { m_bInSingleLineComment = a_bValue; }
	void CState::setMultiLineComment(const bool a_bValue) { m_bInMultiLineComment = a_bValue; }
	void CState::setPreProcess(const size_t a_iPos) { m_iPosPreProcess = a_iPos; }
	void CState::setInclude(const bool a_bValue) { m_bInInclude = a_bValue; }

	void CState::incLine()
	{
		m_iLine++;
	}

	void CState::addInclude(const compileFile::CInclude &a_value)
	{
		m_includes.push_back(a_value);
	}


}