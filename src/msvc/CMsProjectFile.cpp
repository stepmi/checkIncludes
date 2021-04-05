#include "msvc/CMsProjectFile.h"
#include "system/logger.h"
#include "tools/platforms.h"
#include "tools/strings.h"
#include "tools/filesystem.h"
#include "tools/filename.h"
#include "xml/tinyxml2.h"
#include "xml/xmlUtils.h"
#include "MsProjectDefs.h"

namespace msvc
{
	CMsProjectFile::CMsProjectFile(const CParameters &a_parameters) :
		m_parameters(a_parameters)
	{
	}

	bool CMsProjectFile::load(const platform::string &a_wsFile)
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

	bool CMsProjectFile::save(const platform::string &a_wsPath)
	{
		if (m_xml.SaveFile(PLATFORM_TOSTRING(a_wsPath).c_str()) != tinyxml2::XML_SUCCESS)
			return false;
		correctProjectFileXml(a_wsPath);
		return true;
	}

	

	std::string CMsProjectFile::getStdAfx() const
	{
		for (const auto *pElementItemGroup : xmlUtils::getChildElementsByName(*m_pElementProject, sTagItemGroup))
		{
			for (auto *pElementClCompile : xmlUtils::getChildElementsByName(*pElementItemGroup, sTagClCompile))
			{
				if (!xmlUtils::getChildElementsByName(*pElementClCompile, sTagPrecompiledHeader).empty())
				{
					// we expect only one
					return xmlUtils::getAttributeText(*pElementClCompile, sAttInclude);
				}
			}
		}
		return "";
	}

	bool CMsProjectFile::customize(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy)
	{
		removeProgramDatabaseFileName();
		if (setIntermediateDirectory(a_sCompileFile))
		{
			// assuming we have a project, where pch is already on.
			// we can just disable it, but not enable it.
			if (disablePrecompiledHeaders())
			{
				if (setCompileFileWorkingCopy(a_sCompileFile, a_sCompileFileWorkingCopy))
				{									
					removePrecompiledHeaderFromCompiles();
					return true;
				}
			}
		}
		logger::add(logger::EType::eError, "Couldn't read " + PLATFORM_TOSTRING(m_parameters.getProject()) + ". Error in file format.");
		return false;
	}

	bool CMsProjectFile::switchPreProcessOnly(const bool a_bOn)
	{
		auto  *pElementItemDefinitionGroup = findOrAddGroupWithCondition(sTagItemDefinitionGroup);
		if (pElementItemDefinitionGroup)
		{
			auto *pElementClCompile = findOrAddElement(*pElementItemDefinitionGroup, sTagClCompile, &std::string(), "");
			if (pElementClCompile)
			{
				return findOrAddElement(*pElementClCompile, sTagPreprocessToFile, nullptr, a_bOn ? "true" : "false");
			}
		}		
		return false;
	}

	std::string CMsProjectFile::getIntermediateDirectory(const std::string &a_sCompileFile)
	{
		return "checkIncludes_" + tools::filename::getSpecificFileName(a_sCompileFile) + "\\";
	}

	

	std::string CMsProjectFile::createCondition() const
	{
		return "'$(Configuration)|$(Platform)'=='" + m_parameters.getProjectConfiguration().m_sConfiguration + "|" + m_parameters.getProjectConfiguration().m_sPlatform + "'";
	}

	tinyxml2::XMLElement* CMsProjectFile::findOrAddElement(tinyxml2::XMLElement &a_rElement, const std::string &a_sTag, const std::string *a_psValueToFind, const std::string &a_sValueToSet)
	{
		return xmlUtils::findOrAddElement(m_xml, a_rElement, a_sTag, a_psValueToFind, a_sValueToSet);
	}

	tinyxml2::XMLElement* CMsProjectFile::findOrAddGroupWithCondition(const std::string &a_sTagGroup)
	{
		// we search for a tag with only one attribute "condition'
		const std::string sCondition = createCondition();		
		for (auto *pElementGroup : xmlUtils::getChildElementsByName(*m_pElementProject, a_sTagGroup))
		{
			const std::string sConditionCandidate = xmlUtils::getAttributeText(*pElementGroup, sAttCondition);
			if (sConditionCandidate == sCondition)
				return pElementGroup;
		}

		// create property group
		tinyxml2::XMLElement *pNewChildElement = m_xml.NewElement(a_sTagGroup.c_str());
		if (pNewChildElement)
		{
			pNewChildElement->SetAttribute(sAttCondition.c_str(), sCondition.c_str());
			m_pElementProject->InsertEndChild(pNewChildElement);
			return pNewChildElement;
		}
		return nullptr;
	}

	void CMsProjectFile::removeElements(const std::vector<std::string> &a_tagsRecursive, const tinyxml2::XMLElement *a_pElementToKeep)
	{
		auto elementsToRemove = xmlUtils::findElements(m_xml, *m_pElementProject, a_tagsRecursive);
		for (auto *pElementToRemove : elementsToRemove)
		{
			if (pElementToRemove != a_pElementToKeep)			
				m_xml.DeleteNode(pElementToRemove);			
		}
	}

	bool CMsProjectFile::setIntermediateDirectory(const std::string &a_sCompileFile)
	{
		// remove all other IntDir entries in all PropertyGroups
		// cmake creates such entries elsewhere and we don't want msbuild to use them
		// see comment in MsProjectDefs.h about project file structure
		// Maybe we have to do this for other entries too.
		removeElements({ sTagPropertyGroup, sTagIntDir }, nullptr);

		// We have to add a new "PropertyGroup" element just after the last "PropertyGroup".
		// So it is the last one. This is important to overwrite entries from property sheets.
		auto elementsPropertyGroup = xmlUtils::getChildElementsByName(*m_pElementProject, sTagPropertyGroup);
		if (!elementsPropertyGroup.empty())
		{
			// create property group with a condition
			tinyxml2::XMLElement *pNewElementPropertyGroup = m_xml.NewElement(sTagPropertyGroup.c_str());
			if (pNewElementPropertyGroup)
			{
				const std::string sCondition = createCondition();
				pNewElementPropertyGroup->SetAttribute(sAttCondition.c_str(), sCondition.c_str());
				m_pElementProject->InsertAfterChild(elementsPropertyGroup.back(), pNewElementPropertyGroup);
				const std::string sValueIntDir = getIntermediateDirectory(a_sCompileFile);
				auto *pElementIntDir = findOrAddElement(*pNewElementPropertyGroup, sTagIntDir, nullptr, sValueIntDir);
				if (pElementIntDir)
					return true;
			}
		}

		return false;
	}

	bool CMsProjectFile::disablePrecompiledHeaders()
	{
		auto  *pElementItemDefinitionGroup = findOrAddGroupWithCondition(sTagItemDefinitionGroup);
		if (pElementItemDefinitionGroup)
		{
			auto *pElementClCompile = findOrAddElement(*pElementItemDefinitionGroup, sTagClCompile, &std::string(), "");
			if (pElementClCompile)
			{
				return findOrAddElement(*pElementClCompile, sTagPrecompiledHeader, nullptr, "NotUsing");
			}
		}
		return false;
	}

	void CMsProjectFile::removePrecompiledHeaderFromCompiles()
	{
		for (auto *pElementItemGroup : xmlUtils::getChildElementsByName(*m_pElementProject, sTagItemGroup))
		{
			for (auto *pElementClCompile : xmlUtils::getChildElementsByName(*pElementItemGroup, sTagClCompile))
			{
				if (!xmlUtils::getChildElementsByName(*pElementClCompile, sTagPrecompiledHeader).empty())
				{
					// we expect only one
					m_sStdAfx = xmlUtils::getAttributeText(*pElementClCompile, sAttInclude);
					pElementItemGroup->DeleteChild(pElementClCompile);
				}
			}
		}
	}

	bool CMsProjectFile::setCompileFileWorkingCopy(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy)
	{
		for (auto *pElementItemGroup : xmlUtils::getChildElementsByName(*m_pElementProject, sTagItemGroup))
		{
			for (auto *pElementClCompile : xmlUtils::getChildElementsByName(*pElementItemGroup, sTagClCompile))
			{
				if (xmlUtils::getChildElementsByName(*pElementClCompile, sTagPrecompiledHeader).empty())
				{
					const std::string sCompileFile = xmlUtils::getAttributeText(*pElementClCompile, sAttInclude);
					if (sCompileFile == a_sCompileFile)
					{
						pElementClCompile->SetAttribute(sAttInclude.c_str(), a_sCompileFileWorkingCopy.c_str());
						return true;
					}
				}
			}
		}
		return false;
	}

	void CMsProjectFile::removeProgramDatabaseFileName()
	{
		for (auto *pElementItemDefinitionGroup : xmlUtils::getChildElementsByName(*m_pElementProject, sTagItemDefinitionGroup))
		{
			for (auto *pElementClCompile : xmlUtils::getChildElementsByName(*pElementItemDefinitionGroup, sTagClCompile))
			{
				for (auto *pElementProgramDataBaseFileName : xmlUtils::getChildElementsByName(*pElementClCompile, sTagProgramDataBaseFileName))
				{
					pElementClCompile->DeleteChild(pElementProgramDataBaseFileName);
				}
			}
		}
	}

	void CMsProjectFile::correctProjectFileXml(const platform::string &a_wsPath)
	{
		// the msvc-project-file is not xml-compliant
		std::string sFileData = tools::filesystem::readFile(a_wsPath);
		tools::strings::replace(sFileData, std::string("&apos;"), std::string("'"), true);
		tools::filesystem::writeFile(a_wsPath, sFileData);
	}


}