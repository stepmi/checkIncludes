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

		const std::string sQuote = "\"";

		std::string getQuoted(const std::string &a)
		{
			return sQuote + a + sQuote;
		}

		bool strCompareCaseInsensitive(const platform::string &a, const platform::string &b)
		{
			// std::filesystem::path.compare() is case sensitive, so we use this
			return strCompareCaseInsensitive(a.string(), b.string());
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

		
#ifdef _WIN32

		UINT global_iCodePage = 0;

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