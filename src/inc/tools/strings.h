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

		template<typename T> T _tolower_safe(const T c)
		{
			// this should be used for comparing include file names
			// so this should be ascii-characters
			using CHAR_TYPE_UNSIGNED = typename std::make_unsigned<T>::type;
			return static_cast<T>(std::tolower(static_cast<CHAR_TYPE_UNSIGNED>(c)));
		}

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

		// return true, if equal
		template<typename T> bool strCompareCaseInsensitive(const T &a, const T &b)
		{
			if (a.size() == b.size())
			{
				return std::equal(a.begin(), a.end(), b.begin(),
					[](auto a, auto b) { return _tolower_safe(a) == _tolower_safe(b); });
			}
			else
				return false;
		}

		bool strCompareCaseInsensitive(const platform::string &a, const platform::string &b);

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

	}
}
