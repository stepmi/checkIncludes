#pragma once

#include <vector>
#include "tinyxml2.h"

namespace xmlUtils
{

	template<typename T> std::vector<T*> getChildElementsByName(T &a_rElement, const std::string &a_sName)
	{
		std::vector<T*> result;
		for (auto *pElementChild = a_rElement.FirstChildElement(a_sName.c_str()); pElementChild; pElementChild = pElementChild->NextSiblingElement(a_sName.c_str()))
		{
			if (pElementChild)
				result.push_back(pElementChild);
		}
		return result;
	}

	std::string getElementText(const tinyxml2::XMLElement &a_rElement);
	std::string getAttributeText(const tinyxml2::XMLElement &a_rElement, const std::string &a_sAtributeName);	
	tinyxml2::XMLElement* findOrAddElement(tinyxml2::XMLDocument &a_rDocument, tinyxml2::XMLElement &a_rElement, const std::string &a_sTag, const std::string *a_psValueToFind, const std::string &a_sValueToSet);
	std::vector<tinyxml2::XMLElement*> findElements(tinyxml2::XMLDocument &a_rDocument, tinyxml2::XMLElement &a_rElement, const std::vector<std::string> &a_tagsRecursive);
}