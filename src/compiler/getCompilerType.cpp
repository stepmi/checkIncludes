#include "compiler/getCompilerType.h"
#include <filesystem>
#include "tools/strings.h"
#include "tools/filesystem.h"

namespace compiler
{
	ECompilerType getCompilerTypeFromCommandFileName(const std::string &a_sCommandFileName)
	{
		if (tools::strings::compareCaseInsensitive(a_sCommandFileName, "g++") ||
			tools::strings::compareCaseInsensitive(a_sCommandFileName, "c++")) // see comment below - this is a symlink to c++ on Ubuntu
			return ECompilerType::eGpp;
		return ECompilerType::eUnknown;
	}

	ECompilerType getCompilerTypeFromCommand(const std::string &a_sCommand)
	{
		// we should resolve symlinks. 
		// BUT: The resolved filenames would be very hard to decode.
		// on my build system "c++" resolved to "x86_64-linux-gnu-g++-8"
		// So for now we take the easy way and just use the given names, without reslving symlinks.
		// We don't handle too many compiler types for now anyway.
		// const auto sResolvedPath = tools::filesystem::resolveSymLink(std::filesystem::path(a_sCommand));

		const auto sCommandFileName = std::filesystem::path(a_sCommand).stem().string();
		return getCompilerTypeFromCommandFileName(sCommandFileName);
	}

}