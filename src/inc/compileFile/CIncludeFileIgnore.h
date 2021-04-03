#pragma once

#include <string>
#include "CFilePos.h"
#include <vector>

namespace compileFile
{
	class CInclude;

	// a include file, that has to be ignored when checking
	class CIncludeToIgnore
	{
	public:
		CIncludeToIgnore(const std::string &a_sInclude);

		const std::string &getInclude() const;
		bool operator==(const CInclude &a_include) const;
		const FILEPOSITIONS getIgnoreCases() const;

		void addIgnoreCase(const CFilePos &a_filePos);
	private:
		std::string m_sInclude;
		FILEPOSITIONS m_ignoreCases;
	};

	using INCLUDES_TO_IGNORE = std::vector<CIncludeToIgnore>;

}