#pragma once

#include <cstdint>
#include <memory>
#include <limits>

namespace propnet
{
    class StateDataNode
    {
        public:
            /*
            Creates a tree of state nodes from [id_lo, id_hi)
            */
            StateDataNode(std::uint32_t id_lo, std::uint32_t id_hi);
            StateDataNode(const StateDataNode& other, std::uint32_t generation, bool value);
            StateDataNode(const StateDataNode& other, std::uint32_t generation, std::shared_ptr<StateDataNode> left, std::shared_ptr<StateDataNode> right);

            bool get(std::uint32_t getting_id) const;
            std::shared_ptr<StateDataNode> update(std::uint32_t updating_id, std::uint32_t new_generation, bool new_value);
        private:
            static constexpr auto DEFAULT_VALUE {false};
            static constexpr auto MAX_ID {std::numeric_limits<std::uint32_t>::max()};

            static std::shared_ptr<StateDataNode> construct_starting_node(std::uint32_t id_lo, std::uint32_t id_hi);

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
            void update(std::uint32_t updating_id, bool new_value);
        private:
            std::uint32_t size;
            std::uint32_t generation;
            std::shared_ptr<StateDataNode> root;
    };
};
