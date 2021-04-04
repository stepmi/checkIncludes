#pragma once

#include <string>   
#include <vector>
#include "tools/platforms.h"

namespace compileFile
{
	using HANDLE_INCLUDE = size_t;

	// a include entry, that was found in a compile file
	class CInclude
	{
	public:
		CInclude(const HANDLE_INCLUDE a_iHandle, const std::string &a_sFile, const size_t a_iPosStart, const size_t a_iLine, const bool a_bIsSystemFile);

		// queries
		HANDLE_INCLUDE getHandle() const;
		size_t getPos() const;
		size_t getLine() const;
		std::string getFile() const;
		std::string getTextForMessage() const;
		std::string getMarkerVariableForPreProcess() const;
		std::string getMarkerLineForPreProcess() const;

		// manipulators
		void offset(const int a_iValue);
	private:
		std::string m_sFile;

		HANDLE_INCLUDE m_iHandle = std::string::npos;
		size_t m_iPosStart = std::string::npos,			
			m_iLine = std::string::npos;

		bool m_bIsSystemFile = false; // #include<>
		bool m_bIsEnabled = true;
	};

	using INCLUDES = std::vector<CInclude>;
	using INCLUDE_HANDLES = std::vector<HANDLE_INCLUDE>;
	
}