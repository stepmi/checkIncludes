#include "tools/strings.h"
#include <chrono>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif


namespace tools
{
	namespace strings
	{

		static const std::string sQuote = "\"";

		std::string getQuoted(const std::string &a)
		{
			return sQuote + a + sQuote;
		}

		template<typename T> T tolower_safe(const T c)
		{
			// this should be used for comparing include file names
			// so this should be ascii-characters
			using CHAR_TYPE_UNSIGNED = typename std::make_unsigned<T>::type;
			return static_cast<T>(std::tolower(static_cast<CHAR_TYPE_UNSIGNED>(c)));
		}

		std::string tolower(const std::string &a)
		{
			std::string sResult;
			sResult.reserve(a.size());
			for (const auto &c : a)
				sResult.push_back(tolower_safe(c));
			return sResult;
		}


		// return true, if equal
		template<typename T> bool strCompareCaseInsensitive(const T &a, const T &b)
		{
			if (a.size() == b.size())
			{
				return std::equal(a.begin(), a.end(), b.begin(),
					[](auto a, auto b) { return tolower_safe(a) == tolower_safe(b); });
			}
			else
				return false;
		}

		bool compareCaseInsensitive(const char a, const char b)
		{
			return tolower_safe(a) == tolower_safe(b);
		}

		bool compareCaseInsensitive(const platform::string &a, const platform::string &b)
		{
			// std::filesystem::path.compare() is case sensitive, so we use this
			return strCompareCaseInsensitive(a.string(), b.string());
		}

		bool compareCaseInsensitive(const std::string &a, const std::string &b)
		{
			return strCompareCaseInsensitive(a, b);
		}

		std::string::size_type findCaseInsensitive(const std::string &a_sSrc, const std::string &a_sText)
		{
			const std::string src = tolower(a_sSrc);
			const std::string text = tolower(a_sText);
			return src.find(text);
		}

		bool beginsWithCaseInsensitive(const std::string &a_sSrc, const std::string &a_sText)
		{
			if (a_sText.size() <= a_sSrc.size())
			{
				return std::equal(a_sText.begin(), a_sText.end(), a_sSrc.begin(),
					[](auto a, auto b) { return tolower_safe(a) == tolower_safe(b); });
			}
			else
				return false;
		}

		std::string itos2Digits(const size_t a_iValue)
		{
			std::string s = itos(a_iValue);
			if (s.size() == 1)
				return "0" + s;
			return s;
		}

		std::string secondsToHours(const long long a_iSeconds)
		{
			const size_t iHours = a_iSeconds / 3600;
			const size_t iMinutes = (a_iSeconds / 60) - (iHours * 60);
			const size_t iSeconds = (a_iSeconds - (iHours * 3600) - (iMinutes * 60));
			return itos(iHours) + ":" + itos2Digits(iMinutes) + ":" + itos2Digits(iSeconds);
		}

		bool compareUserFileName(const std::string &a_sFileFromProject, const std::string &a_sFileProvidedByUser)
		{
			// this isn't efficient, but that's not so important here
			// correct directory separator
			platform::string sFileFromProject(a_sFileFromProject);
			sFileFromProject.make_preferred();
			platform::string sFileProvidedByUser(a_sFileProvidedByUser);
			sFileProvidedByUser.make_preferred();
			if (!sFileProvidedByUser.has_parent_path())
				sFileFromProject = sFileFromProject.filename();
			return compareCaseInsensitive(sFileFromProject, sFileProvidedByUser);
		}

		
#ifdef _WIN32

		static const UINT global_iCodePage = 0;

		std::wstring atow(const std::string& a_Src)
		{
			if (a_Src.length())
			{
				std::unique_ptr<wchar_t[]> upResultBuf = std::make_unique<wchar_t[]>(a_Src.length() + 1);
				MultiByteToWideChar(global_iCodePage, 0, a_Src.c_str(), -1, upResultBuf.get(), static_cast<int>(a_Src.length()) + 1);
				return std::wstring(upResultBuf.get());
			}
			return std::wstring();
		}

		std::string wtoa(const std::wstring& a_Src)
		{
			if (a_Src.length())
			{
				std::unique_ptr<char[]> upResultBuf = std::make_unique<char[]>(a_Src.length() + 1);
				WideCharToMultiByte(global_iCodePage, 0, a_Src.c_str(), -1, upResultBuf.get(), static_cast<int>(a_Src.length()) + 1, NULL, NULL);
				return std::string(upResultBuf.get());
			}
			return std::string();
		}
#endif

	}
}