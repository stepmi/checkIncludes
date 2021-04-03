#pragma once

#include <string>
#include <filesystem>

#define PLATFORM_T(x) platform::string(x)
#define PLATFORM_TOSTRING(_c) _c.string()
#define STRING_TO_PLATFORM(_c) platform::string(_c)

namespace platform
{
	using string = std::filesystem::path; //  std::wstring;
}
