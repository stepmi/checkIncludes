#pragma once

#include <string>   
#include <vector>

namespace compileFile
{
	class CFilePos
	{
	public:
		CFilePos(const std::string &a_sCompileFile, const size_t a_iLine) :
			m_sCompileFile(a_sCompileFile), m_iLine(a_iLine) {}
	private:
		std::string m_sCompileFile;
		size_t m_iLine = std::string::npos;
	};

	using FILEPOSITIONS = std::vector<CFilePos>;

}