#include "state_data.hpp"

namespace propnet
{
    StateDataNode::StateDataNode(std::uint32_t id_lo, std::uint32_t id_hi) :
        id {(id_lo + id_hi) / 2},
        generation {0},
        left {construct_starting_node(id_lo, id)},
        right {construct_starting_node(id == MAX_ID ? MAX_ID : id + 1, id_hi)},
        value {DEFAULT_VALUE}
    {}

    StateDataNode::StateDataNode(const StateDataNode& other, std::uint32_t generation, bool value) :
        id {other.id},
        generation {generation},
        left {other.left},
        right {other.right},
        value {value}
    {}

    StateDataNode::StateDataNode(const StateDataNode& other, std::uint32_t generation, std::shared_ptr<StateDataNode> left, std::shared_ptr<StateDataNode> right) :
        id {other.id},
        generation {generation},
        left {left},
        right {right},
        value {other.value}
    {}

    bool StateDataNode::get(std::uint32_t getting_id) const
    {
        if (getting_id == id)
        {
            return value;
        }
        else if (getting_id < id)
        {
            /*
            Even though there's no nullptr check,
            it should never deference a nullptr
            since the size/id is static/known beforehand
            */
            return left->get(getting_id);
        }
        else
        {
            return right->get(getting_id);
        }
    }

    std::shared_ptr<StateDataNode> StateDataNode::update(std::uint32_t updating_id, std::uint32_t new_generation, bool new_value)
    {
        /*
        TODO: This could probs be more efficient with less copies
        but sack at the moment at least.

        Same logic for not checking whilst dereferencing
        in get applies here too
        */
        if (updating_id == id)
        {
            return std::make_shared<StateDataNode>(*this, new_generation, new_value);
        }
        else if (updating_id < id) {
            const auto new_left {left->update(updating_id, new_generation, new_value)};
            return std::make_shared<StateDataNode>(*this, new_generation, new_left, right);
        }
        else
        {
            const auto new_right {right->update(updating_id, new_generation, new_value)};
            return std::make_shared<StateDataNode>(*this, new_generation, left, new_right);
        }
    }

    std::shared_ptr<StateDataNode> StateDataNode::construct_starting_node(std::uint32_t id_lo, std::uint32_t id_hi)
    {
        return id_lo >= id_hi ? nullptr : std::make_unique<StateDataNode>(id_lo, id_hi);
    }

    StateData::StateData(std::uint32_t size) :
        size {size},
        generation {0},
        root {size == 0 ? nullptr : std::make_unique<StateDataNode>(0, size)}
    {}

    StateData::StateData(const StateData& other) :
        size {other.size},
        generation {other.generation + 1},
        root {other.root}
    {}

    bool StateData::get(std::uint32_t getting_id) const
    {
        if (root == nullptr)
        {
            throw std::runtime_error {"Tried to access node but root was nulled"};
        }

        return root->get(getting_id);
    }

    void StateData::update(std::uint32_t updating_id, bool new_value)
    {
        if (updating_id >= size)
        {
            std::runtime_error {"Tried to access element outside of state bounds"};
        }

        if (root == nullptr)
        {
            throw std::runtime_error {"Tried to update node but root was nulled"};
        }

        root = root->update(updating_id, generation, new_value);
    }
};
