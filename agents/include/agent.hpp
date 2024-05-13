#pragma once

#include "../../propnet/include/propnet.hpp"

#include <vector>
#include <cstdint>

namespace agents {
    class Agent
    {
        public:
            Agent(const std::vector<std::uint32_t>& sees, const propnet::Propnet& propnet);

            bool get_action(); // TODO: Correct the return type
        protected:
            virtual bool get_action_impl(const std::vector<std::uint32_t>& sees, const std::vector<std::uint32_t>& legals) = 0; // TODO: Presumably this needs to be a strategy?
        private:
            const std::vector<std::uint32_t> sees;
            const propnet::Propnet& propnet;
    };
};
