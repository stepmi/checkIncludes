#pragma once

#include "tools/platforms.h"
#include <string>
#include <set>
#include "tools/find.h"
#include "EProjectType.h"
#include "EOption.h"
#include "CProjectConfiguration.h"

class CParameters
{
public:	
	const platform::string &getProject() const { return m_wsProject; }	
	const CProjectConfiguration &getProjectConfiguration() const { return m_projectConfiguration; }
	
	const std::vector<std::string> &getIgnoreCompileFiles() const { return m_ignoreCompileFiles; }
	const std::vector<std::string> &getIgnoreIncludes() const { return m_ignoreIncludes; }
	const std::vector<std::string> &getCompileFiles() const { return m_compileFiles; }
	bool getHasOption(const EOption a_eOption) const { return tools::find(m_options, a_eOption); }
	EProjectType getProjectType() const { return m_eType; }
	bool getPrintHelp() const { return m_bPrintHelp; }

	void setProject(const platform::string a_wsValue) { m_wsProject = a_wsValue; }
	void setProjectType(const EProjectType a_Value) { m_eType = a_Value; }
	void setProjectConfiguration(const CProjectConfiguration &a_value) { m_projectConfiguration = a_value; }
	void addIgnoreCompileFile(const std::string &a_sValue) { m_ignoreCompileFiles.push_back(a_sValue); }
	void addIgnoreInclude(const std::string &a_sValue) { m_ignoreIncludes.push_back(a_sValue); }
	void addCompileFile(const std::string &a_sValue) { m_compileFiles.push_back(a_sValue); }
	void addOption(const EOption a_eOption) { m_options.insert(a_eOption); }
	void setPrintHelp() { m_bPrintHelp = true; }
	
private:
	platform::string m_wsProject;
	CProjectConfiguration m_projectConfiguration;
	std::vector<std::string> m_ignoreCompileFiles, // option -i
		m_ignoreIncludes, // option -i
		m_compileFiles; // option -o

	OPTIONS m_options;
	bool m_bPrintHelp = false;
	EProjectType m_eType = EProjectType::eUnknown;
};

