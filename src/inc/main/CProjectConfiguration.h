#pragma once

#include <string>

struct CProjectConfiguration
{
	CProjectConfiguration() = default;
	CProjectConfiguration(const std::string &a_sPlatform, const std::string &a_sConfiguration) :
		m_sPlatform(a_sPlatform), m_sConfiguration(a_sConfiguration) {}

	std::string m_sPlatform,  // x64, x86
		m_sConfiguration;	 // Debug, Release
};
