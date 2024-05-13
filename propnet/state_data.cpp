#include "state_data.hpp"

namespace propnet
{
    StateDataNode::StateDataNode(std::uint32_t size) :
        id {size / 2},
        generation {0},
        left {construct_default_node(id - 1)},
        right {construct_default_node(size - id - 1)},
        value {DEFAULT_VALUE}
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
        if (updating_id == id)
        {
            if (new_generation != generation)
            {
                // Create a new node to return
            }
        }
        else if (updating_id < id) {
            // //
            // new = self.left.update(i, start, mid, id, value)
            // if self.id == id:
            //     self.left = new
            //     return self
            // else:
            //     me = TreeNode(start, end, id, create=False)
            //     me.left = new
            //     me.right = self.right
            //     return me
        }
        else
        {
            // //
            // new = self.right.update(i, mid, end, id, value)
            // if self.id == id:
            //     self.right = new
            //     return self
            // else:
            //     me = TreeNode(start, end, id, create=False)
            //     me.left = self.left
            //     me.right = new
            //     return me
        }
        return nullptr;
    }

    std::shared_ptr<StateDataNode> StateDataNode::construct_default_node(std::uint32_t size)
    {
        return size == 0 ? nullptr : std::make_unique<StateDataNode>(size);
    }

    StateData::StateData(std::uint32_t size) :
        size {size},
        generation {0},
        root {size == 0 ? nullptr : std::make_unique<StateDataNode>(size)}
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

    StateData StateData::update(std::uint32_t updating_id, bool new_value)
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
