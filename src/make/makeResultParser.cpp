#include "makeResultParser.h"
#include "tools/strings.h"
#include "system/logger.h"

#define _TEST_COMMAND_LINE

#ifdef _WIN32
	#ifdef _TEST_COMMAND_LINE
		#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
		#include <windows.h>
		#include <shellapi.h>
	#endif
#endif

namespace make
{

	class CState
	{
	public:
		explicit CState(const std::string &a_sText) :
			m_sText(a_sText), m_iSize(m_sText.size())
		{}
		
		// queries
		const std::string &getText() const { return m_sText; }		
		char getChar() const { return m_sText[m_iPos]; }
		bool getIsFinished() const { return m_iPos >= m_iSize; }
		const compileFile::COMMANDLINE &getArguments() const { return m_arguments; }				
		bool getIsInQuote() const { return m_cQuote != 0; }

		// manipulators
		void addToArgument(const char a_Char) { m_sArgument.push_back(a_Char); }		
		void incPos() { m_iPos++; }		
		void setQuote(const char a_cQuote) { m_cQuote = a_cQuote; }

		void finishArgument() 
		{  
			if (!m_sArgument.empty())							
				m_arguments.push_back(std::move(m_sArgument));			
		}

	private:
		size_t m_iPos = 0;
		const std::string &m_sText;
		const size_t m_iSize = std::string::npos;

		std::string m_sArgument;
		char m_cQuote = 0; // to indicate in what kind of quote we are now		

		compileFile::COMMANDLINE m_arguments; // result
	};
	
	void handleChar(CState &a_rState)
	{
		static const char cSpace = ' ';
		static const char cTab = '\t';
		static const char cDoubleQuote = '\"';
		static const char cSingleQuote = '\'';

		const char c = a_rState.getChar();

		if (c == cSpace || c == cTab)
		{
			if (!a_rState.getIsInQuote())			
				a_rState.finishArgument();			
			else			
				a_rState.addToArgument(c);			
		}
		else if (c == cDoubleQuote || c == cSingleQuote)
		{
			if (a_rState.getIsInQuote())
				a_rState.setQuote(0);
			else
				a_rState.setQuote(c);
		}
		else		
			a_rState.addToArgument(c);		

		a_rState.incPos();
	}

	// this should be equivalent to CommandLineToArgvW.
	// split a single command line into arguments, considering white space, tab, quotation.
	// but we don't have to handle all possibilities.
	compileFile::COMMANDLINE splitCommandLine(const std::string &a_sLine)
	{
		CState state(a_sLine);
		while (!state.getIsFinished())
		{
			handleChar(state);
		}
		state.finishArgument();
#ifdef _WIN32
	#ifdef _TEST_COMMAND_LINE
		{
			compileFile::COMMANDLINE commandLineWin;
			if (!a_sLine.empty()) 
			{
				auto wsLine = tools::strings::atow(a_sLine);
				int iArgs = 0;
				auto *pwsArgs = CommandLineToArgvW(wsLine.c_str(), &iArgs);
				if (pwsArgs)
				{
					for (int i = 0; i < iArgs; i++)
					{
						commandLineWin.push_back(tools::strings::wtoa(pwsArgs[i]));
					}
					LocalFree(pwsArgs);
				}
			}
			if (commandLineWin == state.getArguments())
			{
			}
			else
			{
				logger::add(logger::EType::eError, "Error parsing make result");
			}
		}
	#endif
#endif
		return state.getArguments();
	}


	std::vector<std::string> splitIntoLines(const std::string &a_sMakeResult)
	{
		const std::vector<std::string> lineBreaks = 
		{ 
			"\r\n",  // windows
			"\n",	// unix
			"\r"    // mac
		};

		// assuming the complete result has the same linefeeds
		// We're parsing results of make-tools here - so this should always be ok

		for (const auto &sLineBreak : lineBreaks)
		{
			if (a_sMakeResult.find(sLineBreak) != std::string::npos)
			{
				return tools::strings::splitString(a_sMakeResult, sLineBreak);
			}
		}
		// no linebreaks found
		return { a_sMakeResult };
	}

	COMMANDLINES getCommandLinesFromMakeResult(const std::string &a_sMakeResult)
	{
		COMMANDLINES result;
		const auto lines = splitIntoLines(a_sMakeResult);
		for (const auto &sLine : lines)
		{
			if (!sLine.empty())
				result.push_back(splitCommandLine(sLine));
		}
		return result;
	}

}