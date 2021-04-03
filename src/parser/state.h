#pragma once

#include <string>
#include <vector>
#include "compileFile/CIncludeFile.h"

namespace parser
{
	class CState
	{
	public:
		explicit CState(const std::string &a_sText);

		// queries
		const std::string &getText() const;
		size_t getPos() const;
		size_t getLine() const;
		bool getIsFinished() const;

		// where are we?
		const bool getIsInAnyComment() const; // '/*' or '//' found
		const bool getIsInMultiLineComment() const;	 // '/*' found
		const bool getIsInInclude() const; // 'include' found
		const bool getIsInPreProcess() const; // # found
		const bool getIsInAnyQuote() const; // " or < found
		const bool getIsInFileQuote() const;  // " found
		const bool getIsInSystemFileQuote() const; // < found
		size_t getPosPreProcess() const;	
		size_t getPosFileQuote() const; 
		size_t getPosSystemFileQuote() const;

		const compileFile::INCLUDES &getIncludes() const;
		
		// manipulators
		void addToPos(const size_t a_iValue);
		void setFileQuote(const size_t a_iPos);
		void setSystemFileQuote(const size_t a_iPos);
		void setSingleLineComment(const bool a_bValue);
		void setMultiLineComment(const bool a_bValue);
		void setPreProcess(const size_t a_iPos);
		void setInclude(const bool a_bValue);
		void incLine();
		void addInclude(const compileFile::CInclude &a_value);
		
	private:
		bool m_bInSingleLineComment = false,
			m_bInMultiLineComment = false,
			m_bInInclude = false;
		size_t m_iPosFileQuote = std::string::npos,
			m_iPosSystemFileQuote = std::string::npos,
			m_iPosPreProcess = std::string::npos;

		size_t m_iLine = 1,
			m_iPos = 0;
		const std::string &m_sText;
		const size_t m_iSize = std::string::npos;

		compileFile::INCLUDES m_includes;
	};
}