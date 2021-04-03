#include "xml/xmlUtils.h"

namespace xmlUtils
{

	std::string getElementText(const tinyxml2::XMLElement &a_rElement)
	{
		if (a_rElement.GetText())
			return std::string(a_rElement.GetText());
		return std::string();
	}

	std::string getAttributeText(const tinyxml2::XMLElement &a_rElement, const std::string &a_sAtributeName)
	{
		const auto *pAttribute = a_rElement.FindAttribute(a_sAtributeName.c_str());
		if (pAttribute)
			return pAttribute->Value();
		return "";
	}

	tinyxml2::XMLElement* findOrAddElement(tinyxml2::XMLDocument &a_rDocument, tinyxml2::XMLElement &a_rElement, const std::string &a_sTag, const std::string *a_psValueToFind, const std::string &a_sValueToSet)
	{
		for (auto *pChildElement : getChildElementsByName(a_rElement, a_sTag))
		{
			if (!a_psValueToFind || getElementText(*pChildElement) == *a_psValueToFind)
			{
				if (getElementText(*pChildElement) != a_sValueToSet)
					pChildElement->SetText(a_sValueToSet.c_str());
				return pChildElement;
			}
		}

		tinyxml2::XMLElement *pNewChildElement = a_rDocument.NewElement(a_sTag.c_str());
		if (pNewChildElement)
		{
			pNewChildElement->SetText(a_sValueToSet.c_str());
			a_rElement.InsertEndChild(pNewChildElement);
			return pNewChildElement;
		}
		return nullptr;
	}

	void findElements(tinyxml2::XMLDocument &a_rDocument, tinyxml2::XMLElement &a_rElement, const std::vector<std::string> &a_tagsRecursive, std::vector<tinyxml2::XMLElement*> &a_rResult)
	{
		if (!a_tagsRecursive.empty())
		{
			for (auto *pChildElement : getChildElementsByName(a_rElement, a_tagsRecursive.front()))
			{
				if (a_tagsRecursive.size() == 1)
					a_rResult.push_back(pChildElement);
				else
				{
					std::vector<std::string> tags = a_tagsRecursive;
					tags.erase(tags.begin());
					// recurse with current child element
					findElements(a_rDocument, *pChildElement, tags, a_rResult);
				}
			}
		}
	}

	std::vector<tinyxml2::XMLElement*> findElements(tinyxml2::XMLDocument &a_rDocument, tinyxml2::XMLElement &a_rElement, const std::vector<std::string> &a_tagsRecursive)
	{
		std::vector<tinyxml2::XMLElement*> result;
		findElements(a_rDocument, a_rElement, a_tagsRecursive, result);
		return result;
	}

}