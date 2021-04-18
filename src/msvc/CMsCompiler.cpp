#include "msvc/CMsCompiler.h"
#include <string>
#include "system/logger.h"
#include "tools/strings.h"
#include "tools/find.h"
#include "main/CParameters.h"
#include "compileFile/ICompileFile.h"
#include "system/execute.h"
#include <assert.h>

namespace msvc
{
	
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
		
		ECompilerType getType() const override
		{
			return ECompilerType::eMsVc;
		}

		compiler::EResult run(const compileFile::ICompileFile &a_compileFile, const compiler::EAction a_eAction, const CParameters &a_parameters, const compiler::OPTIONS &a_options) const override
		{
			const std::string sCommandLine = getCommandLine(a_compileFile, a_eAction, a_parameters, a_options);
			logger::add(logger::EType::eCommandLines, sCommandLine);

			auto eResult = execute::runOutputToConsole(sCommandLine, platform::string());
			if (eResult == execute::EResult::eOk)
				return compiler::EResult::eOk;
			else if (eResult == execute::EResult::eFailed)
				return compiler::EResult::eFailed; // compile failed
			
			return compiler::EResult::eError; // compile couldn't be started at all
		}

		std::string getCompileFileFromCommandLine(const compileFile::COMMANDLINE &) const override
		{
			assert(0); // doesn't make sense for this compiler
			return std::string();
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
		const std::string sMsBuildPath = execute::getCommandPath("MSBuild.exe");
		if (!sMsBuildPath.empty())
			return std::make_unique<CMsCompiler>(sMsBuildPath);
		else
			logger::add(logger::EType::eError, "Couldn't find MsBuild.exe");
		return std::unique_ptr<compiler::ICompiler>(nullptr);
	}

}