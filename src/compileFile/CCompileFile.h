#pragma once

#include "compileFile/ICompileFile.h"
#include "compileFile/CIncludeFile.h"
#include "compileFile/CCompileFileInfo.h"
#include "main/ECompilerType.h"

namespace compileFile
{
	class CCompileFile : public ICompileFile
	{
	public:
		CCompileFile(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy,
			const platform::string &a_wsProjectFile, const COMMANDLINE &a_commandLine,
			const std::string &a_sSrcCode, const INCLUDES &a_includes);

		~CCompileFile();

		const std::string &getFileWorkingCopy() const override { return m_sCompileFileWorkingCopy; }
		const std::string &getFile() const override { return m_sCompileFile; } // the original filename
		const platform::string &getProjectFileWorkingCopy() const override { return m_wsProjectFile; }
		const COMMANDLINE &getCommandLine() const override { return m_commandLine; }

		INCLUDE_HANDLES getIncludesToCheck() const override;
		INCLUDE_HANDLES getIncludes() const override;
		const CInclude *getInclude(const HANDLE_INCLUDE a_hInclude) const override;
		bool switchInclude(const HANDLE_INCLUDE a_hInclude, const bool a_bSwitchOn) override;

		void filterIncludes(INCLUDES_TO_IGNORE &a_includesToIgnore, const platform::string &a_sPreProcessFile) override;

		// preprocess
		bool addMarkersForPreProcess() override;
		bool removeMarkersForPreProcess() override;

	private:

		platform::string getFilePath() const;

		CInclude *getInclude(const HANDLE_INCLUDE a_hInclude);
		void offsetIncludesAfter(const HANDLE_INCLUDE a_hInclude, const int a_iOffset);
		bool switchIncludeInSrcAndFile(CInclude &a_include, const bool a_bSwitchOn);
		bool switchMarkersForPreProcess(const bool a_bSwitchOn);
		void switchMarkerForPreProcessInSrc(const CInclude &a_include, const bool a_bSwitchOn);

	private:
		std::string m_sCompileFile, // the original name
            m_sCompileFileWorkingCopy;

        platform::string m_wsProjectFile;

        std::string m_sSrcCode;
		INCLUDES m_includes; // this contains all includes. some are set to ignore.

		const std::string m_sDisableInclude = "// disabled by checkIncludes ";
		const int m_iLenDisableInclude = 0;

		COMMANDLINE m_commandLine; // compiler call and its arguments. may be empty for some compiler types.	
	};

	platform::string getCompileFilePath(const std::string &a_sCompileFile, const platform::string &a_wsProjectFile);
}

