#pragma once

#include "platforms.h"
#include <memory>

namespace tools
{
	// to manage an already existing file, which was created by checkIncludes (also indirectly - via compile calls, etc.)
	// These files has to be deleted by us, no matter what happens.
	// May also be a directory.
	class CManagedFile
	{
	public:
		CManagedFile(const platform::string a_sFileName);
		~CManagedFile();
		
		platform::string release();
		std::string getFileName() const;
		
	private:
		platform::string m_sFileName;
	};

	using UP_MANAGED_FILE = std::unique_ptr<CManagedFile>;
}