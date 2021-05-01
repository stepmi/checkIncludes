#include "tools/CManagedFile.h"
#include "tools/filesystem.h"
#include "system/exit.h"

namespace tools
{
	// to manage an already existing file, which has to deleted by us
	CManagedFile::CManagedFile(const platform::string a_sFileName) :
		m_sFileName(a_sFileName)
	{
			exitHandler::add(m_sFileName);
	}

	CManagedFile::~CManagedFile()
	{
		if (!m_sFileName.empty())
		{
			tools::filesystem::removeAll(m_sFileName);
			exitHandler::remove(m_sFileName);
		}
	}

	platform::string CManagedFile::release()
	{
		platform::string sResult = m_sFileName;
		exitHandler::remove(m_sFileName);
		m_sFileName.clear();
		return sResult;
	}

	std::string CManagedFile::getFileName() const
	{
		return m_sFileName.string();
	}

		
}