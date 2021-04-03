#pragma once

#include <set>

enum class EOption
{
	eRequiresPrecompiledHeaders,
	eCompileLog
};

using OPTIONS = std::set<EOption>;

