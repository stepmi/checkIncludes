#include "system/execute.h"
#include <filesystem>
#include "system/logger.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "Shellapi.h"
#endif

namespace execute
{

#ifdef _WIN32
	std::string getLastError()
	{
		DWORD error = GetLastError();
		if (error)
		{
			LPVOID lpMsgBuf;
			DWORD bufLen = FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&lpMsgBuf,
				0, NULL);
			if (bufLen)
			{
				LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
				std::string result(lpMsgStr, lpMsgStr + bufLen);

				LocalFree(lpMsgBuf);

				return result;
			}
		}
		return std::string();
	}
#endif

	EResult run(const std::string &a_sCommandline)
	{
#ifdef _WIN32		
		// 2nd parameter of CreateProcess() is a non-const ptr.						
		const auto iBufSize = a_sCommandline.size() + 1;
		std::unique_ptr<char[]> upCommandLine = std::make_unique<char[]>(iBufSize);
		strcpy_s(upCommandLine.get(), iBufSize, a_sCommandline.c_str());

		STARTUPINFOA sinfo;
		PROCESS_INFORMATION pinfo;
		ZeroMemory(&sinfo, sizeof(STARTUPINFO));
		ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));
		sinfo.cb = sizeof(STARTUPINFOW);

		if (CreateProcessA(NULL, upCommandLine.get(), nullptr, nullptr, false, 0, nullptr, std::filesystem::current_path().string().c_str(), &sinfo, &pinfo))
		{
			WaitForSingleObject(pinfo.hProcess, INFINITE);  // wait for process to end				
			DWORD dwExitCode = 0;
			::GetExitCodeProcess(pinfo.hProcess, &dwExitCode);
			CloseHandle(pinfo.hProcess);
			CloseHandle(pinfo.hThread);

			return dwExitCode == 0 ? EResult::eOk : EResult::eFailed;
		}
		else
		{
			const std::string sError = getLastError();
			logger::add(logger::EType::eError, sError + " with:");
			logger::add(logger::EType::eError, a_sCommandline);
		}		
#endif
		return EResult::eError;
	}
}