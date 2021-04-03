#include "tokens.h"
#include "state.h"

namespace parser
{
	// CToken
	CToken::CToken(const std::string &a_sToken) :
		m_sToken(a_sToken)
	{}

	const std::string &CToken::getToken() const
	{
		return m_sToken;
	}
		
	bool CToken::handle(CState &a_state) const
	{
		if (isToken(a_state))
		{
			handleToken(a_state);
			a_state.addToPos(m_sToken.size());
			return true;
		}
		return false;
	}

	bool CToken::isToken(const CState &a_state) const
	{
		return 0 == a_state.getText().compare(a_state.getPos(), m_sToken.size(), m_sToken);
	}

	// have all tokens on one place, so we can see what we can handle
	const std::string sTokenNumber = "#";
	const std::string sTokenQuote = "\"";
	const std::string sTokenLowerThan = "<";
	const std::string sTokenGreaterThan = ">";
	const std::string sTokenSingleLineComment = "//";
	const std::string sTokenMultiLineCommentStart = "/*";
	const std::string sTokenMultiLineCommentEnd = "*/";
	const std::string sTokenLineFeedWindows = "\r\n";  // linefeeds - we handle them all
	const std::string sTokenLineFeedUnix = "\n";
	const std::string sTokenLineFeedMac = "\r";
	const std::string sTokenInclude = "include";
	const std::string sTokenUniversal = " ";

	// tokens

	class CTokenInclude : public CToken
	{
	public:
		CTokenInclude() : CToken(sTokenInclude) {}
	private:
		void handleToken(CState &a_state) const override
		{		
			if (!a_state.getIsInAnyComment() && 
				!a_state.getIsInInclude() && 
				!a_state.getIsInAnyQuote() &&
				a_state.getIsInPreProcess())
			{
				a_state.setInclude(a_state.getPos());				
			}
		}
	};

	class CTokenNumber : public CToken
	{
	public:
		CTokenNumber() : CToken(sTokenNumber) {}
	private:
		void handleToken(CState &a_state) const override
		{
			if (!a_state.getIsInAnyQuote() &&
				!a_state.getIsInAnyComment())
			{
				a_state.setPreProcess(a_state.getPos());
			}
		}
	};

	class CTokenQuote : public CToken
	{
	public:
		CTokenQuote() : CToken(sTokenQuote) {}
	private:
		void handleToken(CState &a_state) const override
		{						
			if (a_state.getIsInFileQuote())
			{
				if (!a_state.getIsInSystemFileQuote())
				{
					// If we have found a first quote. The next one is the closing quote; everything inbetween is ignored.
					// That's certainly not the case for string literals (thinking of escape sequences).
					// But since we are only interested in include file names, that should be enough
					if (a_state.getIsInInclude())
					{
						//const std::size_t iIncludeLength = a_state.getPos() + getToken().size() - a_state.getPosPreProcess();
						const std::size_t iFileLength = a_state.getPos() - a_state.getPosFileQuote() - 1;
						const std::string sIncludeFile = a_state.getText().substr(a_state.getPosFileQuote() + 1, iFileLength);
						auto hInclude = static_cast<compileFile::HANDLE_INCLUDE>(a_state.getLine()); // there can only be one include per line, so we use that as a handle, which is very readable
						const bool bIsSystemFile = true;
						a_state.addInclude(compileFile::CInclude(hInclude, sIncludeFile, a_state.getPosPreProcess(), /*iIncludeLength,*/ a_state.getLine(), !bIsSystemFile));
						a_state.setPreProcess(std::string::npos);
						a_state.setInclude(false);
					}
					a_state.setFileQuote(std::string::npos);
				}
			}
			else
			{
				if (!a_state.getIsInAnyComment() &&
					!a_state.getIsInSystemFileQuote())
				{
					// accept starting quote only if we are not in a comment
					a_state.setFileQuote(a_state.getPos());
				}
			}
		}		
	};

	class CTokenLowerThan : public CToken // <
	{
	public:
		CTokenLowerThan() : CToken(sTokenLowerThan) {}
	private:
		void handleToken(CState &a_state) const override
		{
			if (!a_state.getIsInAnyComment() &&
				!a_state.getIsInAnyQuote())
			{
				// accept starting quote only if we are not in a comment
				a_state.setSystemFileQuote(a_state.getPos());
			}
		}
	};

	class CTokenGreaterThan : public CToken // >
	{
	public:
		CTokenGreaterThan() : CToken(sTokenGreaterThan) {}
	private:
		void handleToken(CState &a_state) const override
		{
			if (a_state.getIsInSystemFileQuote())
			{
				if (!a_state.getIsInFileQuote())
				{
					if (a_state.getIsInInclude())
					{
						//const std::size_t iIncludeLength = a_state.getPos() + getToken().size() - a_state.getPosPreProcess();
						const std::size_t iFileLength = a_state.getPos() - a_state.getPosSystemFileQuote() - 1;
						const std::string sIncludeFile = a_state.getText().substr(a_state.getPosSystemFileQuote() + 1, iFileLength);
						auto hInclude = static_cast<compileFile::HANDLE_INCLUDE>(a_state.getLine()); // there can only be one include per line, so we use that as a handle, which is very readable
						const bool bIsSystemFile = true;
						a_state.addInclude(compileFile::CInclude(hInclude, sIncludeFile, a_state.getPosPreProcess(), /*iIncludeLength,*/ a_state.getLine(), bIsSystemFile));
						a_state.setPreProcess(std::string::npos);
						a_state.setInclude(false);
					}
					a_state.setSystemFileQuote(std::string::npos);
				}
			}
		}
	};

	class CTokenSingleLineComment : public CToken
	{
	public:
		CTokenSingleLineComment() : CToken(sTokenSingleLineComment) {}
	private:
		void handleToken(CState &a_state) const override
		{
			if (!a_state.getIsInAnyQuote() &&			
				!a_state.getIsInAnyComment())
			{
				a_state.setSingleLineComment(true);
				a_state.setFileQuote(std::string::npos);
				a_state.setSystemFileQuote(std::string::npos);
				a_state.setPreProcess(std::string::npos);
				a_state.setInclude(false);
			}
		}
	};
	
	class CTokenMultiLineCommentStart : public CToken
	{		
	public:   
		CTokenMultiLineCommentStart() : CToken(sTokenMultiLineCommentStart) {}
	private:
		void handleToken(CState &a_state) const override
		{
			if (!a_state.getIsInAnyQuote() &&
				!a_state.getIsInAnyComment())
			{
				a_state.setMultiLineComment(true);
				a_state.setFileQuote(std::string::npos);
				a_state.setSystemFileQuote(std::string::npos);
				a_state.setPreProcess(std::string::npos);
				a_state.setInclude(false);
			}
		}
	};

	class CTokenMultiLineCommentEnd : public CToken
	{
	public:
		CTokenMultiLineCommentEnd() : CToken(sTokenMultiLineCommentEnd) {}
	private:
		void handleToken(CState &a_state) const override
		{ 
			if (a_state.getIsInMultiLineComment())
			{
				a_state.setMultiLineComment(false);
				a_state.setSingleLineComment(false);				
			}
		}
	};

	// base class for all linefeeds
	class CTokenLineFeed : public CToken
	{
	public:
		explicit CTokenLineFeed(const std::string &a_sToken) : CToken(a_sToken)
		{}
	private:
		void handleToken(CState &a_state) const override
		{
			// reset all things that we consider to be single line
			a_state.setSingleLineComment(false);
			a_state.setFileQuote(std::string::npos);
			a_state.setSystemFileQuote(std::string::npos);
			a_state.setPreProcess(std::string::npos);
			a_state.setInclude(false);
			a_state.incLine();
		}
	};

	class CTokenLineFeedWindows : public CTokenLineFeed
	{
	public:
		CTokenLineFeedWindows() : CTokenLineFeed(sTokenLineFeedWindows) {}	
	};
	class CTokenLineFeedUnix : public CTokenLineFeed
	{
	public:
		CTokenLineFeedUnix() : CTokenLineFeed(sTokenLineFeedUnix) {}
	};
	class CTokenLineFeedMac : public CTokenLineFeed
	{
	public:
		CTokenLineFeedMac() : CTokenLineFeed(sTokenLineFeedMac) {}
	};
	
	// this is to be added as the last token.
	// its isToken() method always returns true
	class CTokenUniversal : public CToken
	{
	public:
		CTokenUniversal() : CToken(" ") {}
	private:
		void handleToken(CState &) const override {}
		bool isToken(const CState &) const override { return true; }
	};

	TOKENS getTokens()
	{
		// initializer list is not possible, because it doesn't support move semantics
		TOKENS tokens;
		tokens.push_back(std::make_unique<CTokenInclude>());
		tokens.push_back(std::make_unique<CTokenNumber>());
		tokens.push_back(std::make_unique<CTokenQuote>());
		tokens.push_back(std::make_unique<CTokenLowerThan>());
		tokens.push_back(std::make_unique<CTokenGreaterThan>());
		tokens.push_back(std::make_unique<CTokenSingleLineComment>());
		tokens.push_back(std::make_unique<CTokenMultiLineCommentStart>());
		tokens.push_back(std::make_unique<CTokenMultiLineCommentEnd>());
		tokens.push_back(std::make_unique<CTokenLineFeedWindows>()); // unix and mac contain the same characters, so windows must be the first
		tokens.push_back(std::make_unique<CTokenLineFeedUnix>());
		tokens.push_back(std::make_unique<CTokenLineFeedMac>());		
		tokens.push_back(std::make_unique<CTokenUniversal>());	// this must always be the last one

		return tokens;
	}

}