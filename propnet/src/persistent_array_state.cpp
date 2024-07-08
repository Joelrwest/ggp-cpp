#include "persistent_array_state.hpp"

namespace propnet
{
StateNode::StateNode(std::uint32_t id_lo, std::uint32_t id_hi)
    : id{(id_lo + id_hi) / 2}, generation{0}, left{construct_starting_node(id_lo, id)},
      right{construct_starting_node(id == MAX_ID ? MAX_ID : id + 1, id_hi)}, value{DEFAULT_VALUE}
{
}

StateNode::StateNode(const StateNode &other, std::uint32_t generation, bool value)
    : id{other.id}, generation{generation}, left{other.left}, right{other.right}, value{value}
{
}

StateNode::StateNode(const StateNode &other, std::uint32_t generation, std::shared_ptr<StateNode> left,
                     std::shared_ptr<StateNode> right)
    : id{other.id}, generation{generation}, left{left}, right{right}, value{other.value}
{
}

bool StateNode::get(std::uint32_t getting_id) const
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

std::shared_ptr<StateNode> StateNode::update(std::uint32_t updating_id, std::uint32_t new_generation, bool new_value)
{
    /*
    TODO: This could probs be more efficient with less copies
    but sack at the moment at least.

    Same logic for not checking whilst dereferencing
    in get applies here too
    */
    if (updating_id == id)
    {
        return std::make_shared<StateNode>(*this, new_generation, new_value);
    }
    else if (updating_id < id)
    {
        const auto new_left{left->update(updating_id, new_generation, new_value)};
        return std::make_shared<StateNode>(*this, new_generation, new_left, right);
    }
    else
    {
        const auto new_right{right->update(updating_id, new_generation, new_value)};
        return std::make_shared<StateNode>(*this, new_generation, left, new_right);
    }
}

void StateNode::into_vector(std::vector<bool> &vector)
{
    vector.at(id) = value;

    if (left != nullptr)
    {
        left->into_vector(vector);
    }
    if (right != nullptr)
    {
        right->into_vector(vector);
    }
}

std::shared_ptr<StateNode> StateNode::construct_starting_node(std::uint32_t id_lo, std::uint32_t id_hi)
{
    return id_lo >= id_hi ? nullptr : std::make_unique<StateNode>(id_lo, id_hi);
}

State::State(std::uint32_t size)
    : size{size}, generation{0}, root{size == 0 ? nullptr : std::make_unique<StateNode>(0, size)}, is_initial{true}
{
}

State::State(const State &other)
    : size{other.size}, generation{other.generation + 1}, root{other.root}, is_initial{other.is_initial}
{
}

bool State::get(std::uint32_t getting_id) const
{
    if (root == nullptr)
    {
        throw std::runtime_error{"Tried to access node but root was nulled"};
    }

    return root->get(getting_id);
}

void State::update(std::uint32_t updating_id, bool new_value)
{
    if (updating_id >= size)
    {
        std::runtime_error{"Tried to access element outside of state bounds"};
    }

    if (root == nullptr)
    {
        throw std::runtime_error{"Tried to update node but root was nulled"};
    }

    root = root->update(updating_id, generation, new_value);
}

std::vector<bool> State::to_vector() const
{
    std::vector<bool> vector(size);
    root->into_vector(vector);

    return vector;
}

bool State::get_is_initial() const
{
    return is_initial;
}

void State::set_not_is_initial()
{
    is_initial = false;
}
} // namespace propnet
