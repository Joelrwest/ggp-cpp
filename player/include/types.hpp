#pragma once

#include "../../propnet/include/types.hpp"

#include <unordered_map>
#include <map>

namespace player
{
using Probability = double;
using Policy = std::map<propnet::PropId, Probability>;

using Regret = double;
using Regrets = std::unordered_map<propnet::PropId, Regret>;

using ExpectedValue = double;

using Depth = std::uint16_t;
} // namespace player
