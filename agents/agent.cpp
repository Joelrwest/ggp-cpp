#include "include/agent.hpp"

namespace agents {
    Agent::Agent(const std::vector<std::uint32_t>& sees, const propnet::Propnet& propnet) :
        sees {sees},
        propnet {propnet}
    {}

    bool Agent::get_action()
    {
        // TODO
        return get_action_impl(sees, std::vector<std::uint32_t> {});
    }
};
