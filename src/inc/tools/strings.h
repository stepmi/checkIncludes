#pragma once

#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include "platforms.h"

namespace tools
{
	namespace strings
	{
		std::string getQuoted(const std::string &a);

		
		template<typename T> bool replace(T &a_sMain, const T &a_sFind, const T &a_sReplace, const bool bReplaceAll)
		{
			if (a_sFind.size() == 0)
				return false;

			bool bFound = false;
			typename T::size_type pos = 0;
			do
			{
				pos = a_sMain.find(a_sFind, pos);
				if (pos != T::npos)
				{
					a_sMain.replace(pos, a_sFind.size(), a_sReplace);
					pos = pos + a_sReplace.size();
					bFound = true;
				}
			} while (bReplaceAll && pos != T::npos);

			return bFound;
		}				

		template<typename T> std::string itos(const T a_iVal)
		{
			std::ostringstream os;
			os << a_iVal;
			return os.str();
		}

		std::string secondsToHours(const long long a_iSeconds);

#ifdef _WIN32
		std::wstring atow(const std::string& a_Src);
		std::string wtoa(const std::wstring& a_Src);
#endif
		
		bool compareCaseInsensitive(const char a, const char b);
		bool compareCaseInsensitive(const std::string &a, const std::string &b);
		bool compareCaseInsensitive(const platform::string &a, const platform::string &b);
		std::string::size_type findCaseInsensitive(const std::string &a_sSrc, const std::string &a_sText);
		bool beginsWithCaseInsensitive(const std::string &a_sSrc, const std::string &a_sText);

		bool compareUserFileName(const std::string &a_sFileFromProject, const std::string &a_sFileProvidedByUser);


	}
}
