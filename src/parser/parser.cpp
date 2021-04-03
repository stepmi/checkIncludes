#include "parser/parser.h"
#include <string>
#include "state.h"
#include "tokens.h"


namespace parser
{
	
	compileFile::INCLUDES findIncludes(const std::string &a_sCode)
	{		
		const TOKENS tokens = getTokens();
		CState state(a_sCode);		
		while (!state.getIsFinished())
		{
			for (auto &upToken : tokens)
			{
				// compare
				if (upToken->handle(state))									
					break;				
			}
		}
		return state.getIncludes();
	}


}