#pragma once

#include "tools/platforms.h"
#include <memory>
#include "xml/tinyxml2.h"
#include "main/CProjectConfiguration.h"
#include "compileFile/CCompileFileInfo.h"


namespace msvc
{
	class CMsProjectFileQuery
	{
	public:
		bool load(const platform::string &a_wsFile);
		compileFile::COMPILE_FILES getCompileFiles() const;
		std::vector<CProjectConfiguration> getProjectConfigurations() const;
		
	private:
		tinyxml2::XMLDocument m_xml;
		tinyxml2::XMLElement *m_pElementProject = nullptr; // if load() was successful, this is guaranteed to be valid	
	};	
	
}