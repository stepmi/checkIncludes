#include "system/execute.h"
#include <filesystem>
#include "system/logger.h"
#include "tools/filename.h"
#include "tools/filesystem.h"
#include <array>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "Shellapi.h"
#else
#include <cstdio>
#include <iostream>
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

	class CTempFile
	{
	public:
		CTempFile(const platform::string &a_sFileName, const ::HANDLE a_handle) :
			m_sFileName(a_sFileName), m_handle(a_handle)
		{}

		~CTempFile()
		{
			CloseHandle(m_handle);
			tools::filesystem::removeAll(m_sFileName);
		}

		::HANDLE getHandle() const
		{
			return m_handle;
		}

		std::string readFile() const
		{
			return tools::filesystem::readFile(m_sFileName);
		}

	private:
		::HANDLE m_handle;
		platform::string m_sFileName;
	};

	std::unique_ptr<CTempFile> createTempFile()
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = nullptr;
		sa.bInheritHandle = TRUE;

		const auto sFileName = tools::filename::getTempFileName();
		::HANDLE handle = CreateFileA(sFileName.string().c_str(),
			FILE_APPEND_DATA,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			&sa,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (handle == INVALID_HANDLE_VALUE)
			return std::unique_ptr<CTempFile>(nullptr);

		return std::make_unique<CTempFile>(sFileName, handle);
	}

	EResult run(const std::string &a_sCommandline, const platform::string &a_sWorkingDir, const CTempFile *a_pTempFile)
	{
		// 2nd parameter of CreateProcess() is a non-const ptr (only for wstring - so we could remove this)
		const auto iBufSize = a_sCommandline.size() + 1;
		std::unique_ptr<char[]> upCommandLine = std::make_unique<char[]>(iBufSize);
		strcpy_s(upCommandLine.get(), iBufSize, a_sCommandline.c_str());

		STARTUPINFOA sinfo;
		PROCESS_INFORMATION pinfo;
		ZeroMemory(&sinfo, sizeof(STARTUPINFO));
		ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));
		sinfo.cb = sizeof(STARTUPINFOW);
		if (a_pTempFile)
		{
			sinfo.dwFlags |= STARTF_USESTDHANDLES;
			sinfo.hStdInput = INVALID_HANDLE_VALUE;
			sinfo.hStdError = a_pTempFile->getHandle();
			sinfo.hStdOutput = a_pTempFile->getHandle();
		}

		if (CreateProcessA(NULL, upCommandLine.get(), nullptr, nullptr, true, 0, nullptr, a_sWorkingDir.string().c_str(), &sinfo, &pinfo))
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
			return EResult::eError;
		}
	}

	EResult runOutputToString(const std::string &a_sCommandline, const platform::string &a_sWorkingDir, std::string &a_rsStdOut)
	{
		EResult eResult = EResult::eError;

		platform::string sWorkingDir = a_sWorkingDir;
		if (sWorkingDir.empty())
			sWorkingDir = std::filesystem::current_path();

		auto upTempFile = createTempFile();
		if (upTempFile)
		{
			eResult = run(a_sCommandline, sWorkingDir, upTempFile.get());
			a_rsStdOut = upTempFile->readFile();
		}
		return eResult;
	}


	EResult runOutputToConsole(const std::string &a_sCommandline, const platform::string &a_sWorkingDir)
	{
		platform::string sWorkingDir = a_sWorkingDir;
		if (sWorkingDir.empty())
			sWorkingDir = std::filesystem::current_path();
		return run(a_sCommandline, sWorkingDir, nullptr);
	}

#else

	EResult runOutputToString(const std::string &a_sCommandline, const platform::string &a_sWorkingDir, std::string &a_rsStdOut)
	{
		EResult eResult = EResult::eError;

		const size_t iBufSize = 1000;
		const size_t iReadSize = iBufSize - 1;
		std::array<char, iBufSize> buffer;
		std::unique_ptr<FILE, decltype(&pclose)> upPipe(popen(a_sCommandline.c_str(), "r"), pclose);
		if (upPipe)
		{
			while(true)
			{
                const size_t iRead = fread(buffer.data(), 1, iReadSize, upPipe.get());
				if (iRead != 0)
				{
					buffer[iRead] = 0;
					a_rsStdOut += buffer.data();
				}
				if (iRead != iReadSize)
					break;
			}
			// TODO
			return EResult::eOk;
		}

		logger::add(logger::EType::eError, a_sCommandline + " failed.");
		return EResult::eError;
	}

	EResult runOutputToConsole(const std::string &a_sCommandline, const platform::string &a_sWorkingDir)
	{
		std::unique_ptr<FILE, decltype(&pclose)> upPipe(popen(a_sCommandline.c_str(), "r"), pclose);
		if (upPipe)
		{
			// TODO
			return EResult::eOk;
		}
		logger::add(logger::EType::eError, a_sCommandline + " failed.");
		return EResult::eError;
	}
#endif






	std::string getCommandPath(const std::string &a_sCommand)
	{
#ifdef _WIN32
		LPSTR lpFilePart = nullptr;
		char sFileName[MAX_PATH];
		if (SearchPathA(NULL, std::filesystem::path(a_sCommand).stem().string().c_str(),
			std::filesystem::path(a_sCommand).extension().string().c_str(),
			MAX_PATH, sFileName, &lpFilePart))
		{
			return sFileName;
		}
		else
			return std::string();
#else
		return a_sCommand;
#endif


	}
}
