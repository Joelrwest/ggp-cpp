#pragma once

#include "../../propnet/include/types.hpp"

#include <unordered_map>

namespace player
{
using Probability = double;
using Policy = std::unordered_map<propnet::PropId, Probability>;

using Regret = double;
using Regrets = std::unordered_map<propnet::PropId, Regret>;

using ExpectedValue = double;
} // namespace player
