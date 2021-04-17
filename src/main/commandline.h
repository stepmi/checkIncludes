#pragma once

#include "main/CParameters.h"

std::unique_ptr<CParameters> parseCommandLine(const std::vector<std::string> &a_arguments);
