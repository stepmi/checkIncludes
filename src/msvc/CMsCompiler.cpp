#include "msvc/CMsCompiler.h"
#include <string>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "Shellapi.h"
#include "system/logger.h"
#include "tools/strings.h"
#include "tools/find.h"
#include "main/CParameters.h"
#include "compileFile/ICompileFile.h"

namespace msvc
{
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

	bool getHasOption(const compiler::OPTIONS &a_options, const compiler::EOption a_eOption)
	{
		return tools::find(a_options, a_eOption);
	}

	class CMsCompiler : public compiler::ICompiler
	{
	public:
		explicit CMsCompiler(const std::string &a_wsMsBuildPath) :
			m_wsMsBuildPath(a_wsMsBuildPath)
		{}
		
		compiler::EResult run(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, const compiler::OPTIONS &a_options) const override
		{
			const std::string sCommandLine = getCommandLine(a_compileFile, a_eAction, a_parameters, a_options);
			logger::add(logger::EType::eCommandLines, sCommandLine);
			// 2nd parameter of CreateProcess() is a non-const ptr.						
			const auto iBufSize = sCommandLine.size() + 1;
			std::unique_ptr<char[]> upCommandLine = std::make_unique<char[]>(iBufSize);
			strcpy_s(upCommandLine.get(), iBufSize, sCommandLine.c_str());

			STARTUPINFOA sinfo;
			PROCESS_INFORMATION pinfo;
			ZeroMemory(&sinfo, sizeof(STARTUPINFO));
			ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));
			sinfo.cb = sizeof(STARTUPINFOW);

			const BOOL b = CreateProcessA(NULL, upCommandLine.get(), nullptr, nullptr, false, 0, nullptr, std::filesystem::current_path().string().c_str(), &sinfo, &pinfo);
			if (b)
			{
				WaitForSingleObject(pinfo.hProcess, INFINITE);  // wait for process to end				
				DWORD dwExitCode = 0;
				::GetExitCodeProcess(pinfo.hProcess, &dwExitCode);
				CloseHandle(pinfo.hProcess);
				CloseHandle(pinfo.hThread);

				return dwExitCode == 0 ? compiler::EResult::eOk : compiler::EResult::eFailed;
			}
			else
			{
				const std::string sError = getLastError();				
				logger::add(logger::EType::eError, sError + " with:");
				logger::add(logger::EType::eError, sCommandLine);				
				return compiler::EResult::eError;
			}
		}
	private:
		std::string getTargetText(const compiler::EAction a_eAction) const
		{
			std::string ws = "/t:";
			if (a_eAction == compiler::EAction::eCompile)
				return ws + "ClCompile";
			else if (a_eAction == compiler::EAction::eReBuild)
				return ws + "Clean;ClCompile";
			else
				return "";
		}

		std::string getOptionsText(const compiler::OPTIONS &a_options) const
		{
			std::string ws;
			if (getHasOption(a_options, compiler::EOption::eLogErrors))
				ws += "/nologo /verbosity:quiet";
			else if (!getHasOption(a_options, compiler::EOption::eLogAll))
				ws += "/nologo /noconlog"; // / m";			
			return ws;
		}

		std::string getConfiguration(const CParameters &a_parameters) const
		{
			return "/p:Configuration=" + a_parameters.getProjectConfiguration().m_sConfiguration + ";Platform=" + a_parameters.getProjectConfiguration().m_sPlatform;
		}

		std::string getAdditionalProperties(const compiler::EAction a_eAction) const
		{
			return "";
		}

		std::string getCommandLine(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, const compiler::OPTIONS &a_options) const
		{
			return getQuoted(m_wsMsBuildPath) + m_wsWhiteSpace +
				getQuoted(a_compileFile.getProjectFileWorkingCopy().string()) + m_wsWhiteSpace +
				getConfiguration(a_parameters) + m_wsWhiteSpace +
				getTargetText(a_eAction) + m_wsWhiteSpace +
				getOptionsText(a_options) + m_wsWhiteSpace +
				getAdditionalProperties(a_eAction) + m_wsWhiteSpace +
				"/p:SelectedFiles=" + getQuoted(a_compileFile.getFileWorkingCopy());
		}

		static std::string getQuoted(const std::string &a)
		{
			const std::string m_wsQuote = "\"";
			return m_wsQuote + a + m_wsQuote;
		}

	private:
		const std::string m_wsWhiteSpace = " ";
		std::string m_wsMsBuildPath;
	};

	std::unique_ptr<compiler::ICompiler> createMsCompiler()
	{
		LPSTR lpFilePart = nullptr;
		char sFileName[MAX_PATH];

		if (SearchPathA(NULL, "MSBuild", ".exe", MAX_PATH, sFileName, &lpFilePart))
		{
			return std::make_unique<CMsCompiler>(sFileName);
		}
		else
		{
			logger::add(logger::EType::eError, "Couldn't find MsBuild.exe");
		}	
		return std::unique_ptr<compiler::ICompiler>(nullptr);
	}

}