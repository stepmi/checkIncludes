#pragma once

#include "tools/platforms.h"
#include "main/CParameters.h"

std::unique_ptr<CParameters> parseCommandLine(const std::vector<platform::string> &a_arguments);
