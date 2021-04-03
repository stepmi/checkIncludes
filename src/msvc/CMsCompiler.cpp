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
	std::wstring getLastError()
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

				return tools::strings::atow(result);
			}
		}
		return std::wstring();
	}

	bool getHasOption(const compiler::OPTIONS &a_options, const compiler::EOption a_eOption)
	{
		return tools::find(a_options, a_eOption);
	}

	class CMsCompiler : public compiler::ICompiler
	{
	public:
		explicit CMsCompiler(const std::wstring &a_wsMsBuildPath) :
			m_wsMsBuildPath(a_wsMsBuildPath)
		{}
		
		compiler::EResult run(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, const compiler::OPTIONS &a_options) const override
		{
			const std::wstring wsCommandLine = getCommandLine(a_compileFile, a_eAction, a_parameters, a_options);
			//logger::add(logger::EType::eMessage, tools::strings::wtoa(wsCommandLine));
			// 2nd parameter of CreateProcess() is a non-const ptr.						
			const auto iBufSize = wsCommandLine.size() + 1;
			std::unique_ptr<wchar_t[]> upCommandLine = std::make_unique<wchar_t[]>(iBufSize);
			wcscpy_s(upCommandLine.get(), iBufSize, wsCommandLine.c_str());

			STARTUPINFOW sinfo;
			PROCESS_INFORMATION pinfo;
			ZeroMemory(&sinfo, sizeof(STARTUPINFO));
			ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));
			sinfo.cb = sizeof(STARTUPINFOW);

			const BOOL b = CreateProcessW(NULL, upCommandLine.get(), nullptr, nullptr, false, 0, nullptr, std::filesystem::current_path().wstring().c_str(), &sinfo, &pinfo);
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
				std::wstring wsError = getLastError();
				logger::add(logger::EType::eError, tools::strings::wtoa(wsError));
				return compiler::EResult::eError;
			}
		}
	private:
		std::wstring getTargetText(const compiler::EAction a_eAction) const
		{
			std::wstring ws = L"/t:";
			if (a_eAction == compiler::EAction::eCompile)
				return ws + L"ClCompile";
			else if (a_eAction == compiler::EAction::eReBuild)
				return ws + L"Clean;ClCompile";
			//else if (a_eAction == EAction::ePreCompile)
			//	return ws + L"ClCompile";
			else
				return L"";
		}

		// seems that msbuild doesn't accept most things as properties.
		// so especially PreprocessToFile didn't work

		std::wstring getOptionsText(const compiler::OPTIONS &a_options) const
		{
			std::wstring ws;
			if (getHasOption(a_options, compiler::EOption::eLogErrors))
				ws += L"/nologo /verbosity:quiet";
			else if (!getHasOption(a_options, compiler::EOption::eLogAll))
				ws += L"/nologo /noconlog"; // / m";			
			return ws;
		}

		std::wstring getConfiguration(const CParameters &a_parameters) const
		{
			return L"/p:Configuration=" + tools::strings::atow(a_parameters.getProjectConfiguration().m_sConfiguration) + L";Platform=" + tools::strings::atow(a_parameters.getProjectConfiguration().m_sPlatform);
		}

		std::wstring getAdditionalProperties(const compiler::EAction a_eAction) const
		{
			return L"";
		}

		std::wstring getCommandLine(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, const compiler::OPTIONS &a_options) const
		{
			return getQuoted(m_wsMsBuildPath) + m_wsWhiteSpace +
				getQuoted(a_compileFile.getProjectFileWorkingCopy()) + m_wsWhiteSpace +
				getConfiguration(a_parameters) + m_wsWhiteSpace +
				getTargetText(a_eAction) + m_wsWhiteSpace +
				getOptionsText(a_options) + m_wsWhiteSpace +
				getAdditionalProperties(a_eAction) + m_wsWhiteSpace +
				L"/p:SelectedFiles=" + getQuoted(tools::strings::atow(a_compileFile.getFileWorkingCopy()));
		}

		static std::wstring getQuoted(const std::wstring &a)
		{
			const std::wstring m_wsQuote = L"\"";
			return m_wsQuote + a + m_wsQuote;
		}

	private:
		const std::wstring m_wsWhiteSpace = L" ";
		std::wstring m_wsMsBuildPath;
	};

	std::unique_ptr<compiler::ICompiler> createMsCompiler()
	{
		LPWSTR lpFilePart = nullptr;
		wchar_t wsFileName[MAX_PATH];

		if (SearchPathW(NULL, L"MSBuild", L".exe", MAX_PATH, wsFileName, &lpFilePart))
		{
			return std::make_unique<CMsCompiler>(wsFileName);
		}
		else
		{
			logger::add(logger::EType::eError, "Couldn't find MsBuild.exe");
		}	
		return std::unique_ptr<compiler::ICompiler>(nullptr);
	}

}