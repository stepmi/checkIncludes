#pragma once

#include "main/CParameters.h"
#include "xml/tinyxml2.h"

namespace msvc
{

	class CMsProjectFile
	{
	public:
		CMsProjectFile(const CParameters &a_parameters);

		bool load(const platform::string &a_wsFile);
		bool save(const platform::string &a_wsPath);
				
		bool customize(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy, const bool a_bUsePrecompiledHeaders);
		bool switchPreProcessOnly(const bool a_bOn);		
		std::string getStdAfx() const;

		static std::string getIntermediateDirectory(const std::string &a_sCompileFile);
	private:

		std::string createCondition() const;
		tinyxml2::XMLElement* findOrAddElement(tinyxml2::XMLElement &a_rElement, const std::string &a_sTag, const std::string *a_psValueToFind, const std::string &a_sValueToSet);
		tinyxml2::XMLElement* findOrAddGroupWithCondition(const std::string &a_sTagGroup);
		void removeElements(const std::vector<std::string> &a_tagsRecursive, const tinyxml2::XMLElement *a_pElementToKeep);

		bool setIntermediateDirectory(const std::string &a_sCompileFile);
		bool disablePrecompiledHeaders();
		void removePrecompiledHeaderFromCompiles();
		bool setCompileFileWorkingCopy(const std::string &a_sCompileFile, const std::string &a_sCompileFileWorkingCopy);
		void removeProgramDatabaseFileName();

		static void correctProjectFileXml(const platform::string &a_wsPath);
		
	private:
		const CParameters m_parameters;
		tinyxml2::XMLDocument m_xml;
		tinyxml2::XMLElement *m_pElementProject = nullptr; // if load() was successful, this is guaranteed to be valid	

		// results		
		std::string m_sStdAfx;
	};


}
