#include "msvc/CMsProjectFileQuery.h"
#include "xml/tinyxml2.h"
#include "xml/xmlUtils.h"
#include "MsProjectDefs.h"

namespace msvc
{

	std::vector<std::string> CMsProjectFileQuery::getCompileFiles() const
	{
		std::vector<std::string> result;
		for (const auto *pElementItemGroup : xmlUtils::getChildElementsByName(*m_pElementProject, sTagItemGroup))
		{
			for (auto *pElementClCompile : xmlUtils::getChildElementsByName(*pElementItemGroup, sTagClCompile))
			{
				if (xmlUtils::getChildElementsByName(*pElementClCompile, sTagPrecompiledHeader).empty())
				{
					const std::string sCompileFile = xmlUtils::getAttributeText(*pElementClCompile, sAttInclude);
					if (!sCompileFile.empty())
						result.push_back(sCompileFile);
				}
			}
		}
		return result;
	}

	bool CMsProjectFileQuery::load(const platform::string &a_wsFile)
	{
		if (m_xml.LoadFile(PLATFORM_TOSTRING(a_wsFile).c_str()) == tinyxml2::XML_SUCCESS)
		{
			tinyxml2::XMLElement *pRootElement = m_xml.FirstChildElement(sTagProject.c_str());
			if (pRootElement && !pRootElement->NextSiblingElement(sTagProject.c_str()))
			{
				m_pElementProject = pRootElement;
				return true;
			}
		}
		return false;
	}
	
	std::vector<CProjectConfiguration> CMsProjectFileQuery::getProjectConfigurations() const
	{
		std::vector<CProjectConfiguration> result;
		for (const auto *pElementItemGroup : xmlUtils::getChildElementsByName(*m_pElementProject, sTagItemGroup))
		{
			for (auto *pElementProjectConfiguration : xmlUtils::getChildElementsByName(*pElementItemGroup, sTagProjectConfiguration))
			{
				auto configurations = xmlUtils::getChildElementsByName(*pElementProjectConfiguration, sTagConfiguration);
				auto platforms = xmlUtils::getChildElementsByName(*pElementProjectConfiguration, sTagPlatform);
				if (configurations.size() == 1 && platforms.size() == 1)
				{
					result.emplace_back(CProjectConfiguration(xmlUtils::getElementText(*platforms.front()), 
						xmlUtils::getElementText(*configurations.front())));				
				}
			}
		}

		return result;
	}

}