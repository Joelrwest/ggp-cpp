#pragma once

#include <cstdint>
#include <memory>

namespace propnet
{
    class StateDataNode
    {
        public:
            StateDataNode(std::uint32_t size);

            bool get(std::uint32_t getting_id) const;
            std::shared_ptr<StateDataNode> update(std::uint32_t updating_id, std::uint32_t new_generation, bool new_value);
        private:
            static constexpr auto DEFAULT_VALUE {false};

            static std::shared_ptr<StateDataNode> construct_default_node(std::uint32_t size);

            std::uint32_t id;
            std::uint32_t generation;
            std::shared_ptr<StateDataNode> left;
            std::shared_ptr<StateDataNode> right;
            bool value;
    };

    class StateData
    {
        public:
            StateData(std::uint32_t size);
            StateData(const StateData& other);

            bool get(std::uint32_t getting_id) const;
            StateData update(std::uint32_t updating_id, bool new_value);
        private:
            std::uint32_t size;
            std::uint32_t generation;
            std::shared_ptr<StateDataNode> root;
    };
};
