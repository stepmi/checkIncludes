#pragma once

#include "tools/platforms.h"
#include <string>
#include <set>
#include "tools/find.h"
#include "ECompilerType.h"
#include "EOption.h"
#include "CProjectConfiguration.h"

class CParameters
{
public:
	CParameters(const platform::string &a_wsProject) :
		m_wsProject(a_wsProject)
	{}

	const platform::string &getProject() const { return m_wsProject; }	
	const CProjectConfiguration &getProjectConfiguration() const { return m_projectConfiguration; }
	const std::vector<std::string> &getIgnoreFiles() const { return m_ignoreFiles; }
	const std::vector<std::string> &getCompileFiles() const { return m_compileFiles; }
	bool getHasOption(const EOption a_eOption) const { return tools::find(m_options, a_eOption); }

	void setProjectConfiguration(const CProjectConfiguration &a_value) { m_projectConfiguration = a_value; }
	void addIgnoreFile(const std::string &a_sValue) { m_ignoreFiles.push_back(a_sValue); }
	void addCompileFile(const std::string &a_sValue) { m_compileFiles.push_back(a_sValue); }
	void addOption(const EOption a_eOption) { m_options.insert(a_eOption); }
	ECompilerType getCompilerType() const { return ECompilerType::eMsVc; }
private:
	platform::string m_wsProject;
	CProjectConfiguration m_projectConfiguration;
	std::vector<std::string> m_ignoreFiles,
		m_compileFiles; // option "-o"

	OPTIONS m_options;
};

