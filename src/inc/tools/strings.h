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

		template<typename T> std::vector<T> splitString(const T& a_sSource, const T& a_sSeparator)
		{
			if (a_sSeparator.empty())			
				return { a_sSource };
			if (a_sSource.empty())
				return {};

			std::vector<T> result;
			typename T::size_type iPosStart = 0;
			while (true)
			{
				const typename T::size_type iPosSeperator = a_sSource.find(a_sSeparator, iPosStart);
				if (T::npos != iPosSeperator)
				{
					result.emplace_back(a_sSource.substr(iPosStart, iPosSeperator - iPosStart));
					iPosStart = iPosSeperator + a_sSeparator.length();
				}
				else
				{
					result.emplace_back(a_sSource.substr(iPosStart));
					break;
				}
			}
			return result;
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
