#include "system/execute.h"
#include <filesystem>
#include "system/logger.h"
#include "tools/filename.h"
#include "tools/filesystem.h"
#include <array>
#include "system/exit.h"
#include "tools/CManagedFile.h"
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
	#include <Shellapi.h>
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
			LPVOID lpMsgBuf = nullptr;
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
			m_managedFile(a_sFileName), m_handle(a_handle)
		{
		}

		~CTempFile()
		{
			CloseHandle(m_handle);
		}

		::HANDLE getHandle() const
		{
			return m_handle;
		}

		std::string readFile() const
		{
			return tools::filesystem::readFile(m_managedFile.getFileName());
		}

	private:
		::HANDLE m_handle;
		tools::CManagedFile m_managedFile;
	};

	std::unique_ptr<CTempFile> createTempFile()
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = nullptr;
		sa.bInheritHandle = TRUE;

		const auto sFileName = tools::filename::getTempFileName(".txt");
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
		bool bInheritHandles = false;
		if (a_pTempFile)
		{
			sinfo.dwFlags |= STARTF_USESTDHANDLES;
			sinfo.hStdInput = INVALID_HANDLE_VALUE;
			sinfo.hStdError = a_pTempFile->getHandle();
			sinfo.hStdOutput = a_pTempFile->getHandle();
			// Be very careful with bInheritHandles. 
			// I don't fully understand the purpose, but it seems like the parent process inherits handles of files created by the child process.
			// So maybe we'd have to close such inherited file handles.
			// Resulting problems: msvc-compiler couldn't access some files created by a former build process.
			// For the moment we just don't use it when calling the compiler.
			bInheritHandles = true; 
		}

		if (CreateProcessA(NULL, upCommandLine.get(), nullptr, nullptr, bInheritHandles, 0, nullptr, a_sWorkingDir.string().c_str(), &sinfo, &pinfo))
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

	EResult runQuiet(const std::string &a_sCommandline, const platform::string &a_sWorkingDir)
	{
		std::string sStdOut;
		return runOutputToString(a_sCommandline, a_sWorkingDir, sStdOut);
	}

#else

	class CPipe
	{
	public:
		CPipe(FILE *a_pFile) :
			m_pFile(a_pFile)
		{}

		~CPipe()
		{
			if (m_pFile)
				pclose(m_pFile);
		}

		std::string getData()
		{
			std::string result;
			const size_t iBufSize = 1000;
			const size_t iReadSize = iBufSize - 1;
			std::array<char, iBufSize> buffer;
			while (true)
			{
				const size_t iRead = fread(buffer.data(), 1, iReadSize, m_pFile);
				if (iRead != 0)
				{
					buffer[iRead] = 0;
					result += buffer.data();
				}
				if (iRead != iReadSize)
					break;
			}
			return result;
		}

		EResult close()
		{
			EResult result = EResult::eError;
			if (m_pFile)
			{
				int iStatus = pclose(m_pFile);
				if (WIFEXITED(iStatus))
				{
					//If you need to do something when the pipe exited, this is the time.
					iStatus = WEXITSTATUS(iStatus);
					result = iStatus == 0 ? EResult::eOk : EResult::eFailed;
				}
				m_pFile = nullptr;
			}
			return result;
		}

	private:
		FILE *m_pFile = nullptr;
	};

	std::unique_ptr<CPipe> createPipe(const std::string &a_sCommandline)
	{
		FILE* pFile = popen(a_sCommandline.c_str(), "r");
		if (pFile)
			return std::make_unique<CPipe>(pFile);
		return std::unique_ptr<CPipe>(nullptr);
	}

	EResult runOutputToString(const std::string &a_sCommandline, const platform::string &, std::string &a_rsStdOut)
	{
		auto upPipe = createPipe(a_sCommandline);
		if (upPipe)
		{
			a_rsStdOut = upPipe->getData();
			return upPipe->close();
		}

		logger::add(logger::EType::eError, a_sCommandline + " failed.");
		return EResult::eError;
	}

	EResult runOutputToConsole(const std::string &a_sCommandline, const platform::string &)
	{
		auto upPipe = createPipe(a_sCommandline);
		if (upPipe)
			return upPipe->close();

		logger::add(logger::EType::eError, a_sCommandline + " failed.");
		return EResult::eError;
	}

	EResult runQuiet(const std::string &a_sCommandline, const platform::string &)
	{
		auto upPipe = createPipe(a_sCommandline + " 2>&1 >/dev/null");
		if (upPipe)
			return upPipe->close();

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

	std::string createCommandFromCommandLine(const compileFile::COMMANDLINE &a_commandLine)
	{
		// TODO: quotes may be removed - maybe we have to search for space or tabs in arguments and add quotes
		std::string result;
		for (const auto &sArgument : a_commandLine)
		{
			result += sArgument;
			if (&sArgument != &a_commandLine.back())
				result += " ";
		}
		return result;
	}

}
