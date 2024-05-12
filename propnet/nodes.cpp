#include "nodes.hpp"

namespace propnet
{
    Node::Node(std::uint32_t id) :
        id {id}
    {}

    std::uint32_t Node::get_id() const
    {
        return id;
    }

    AndNode::AndNode(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<uint32_t> outs) :
        Node {id},
        ins {ins},
        outs {outs}
    {}

    std::uint32_t AndNode::evaluate(const std::vector<std::uint32_t> data) const
    {
        for (const auto in : ins)
        {
            if (!data.at(in))
            {
                return 0;
            }
        }
        return 1;
    }

    OrNode::OrNode(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<uint32_t> outs) :
        Node {id},
        ins {ins},
        outs {outs}
    {}

    std::uint32_t OrNode::evaluate(const std::vector<std::uint32_t> data) const
    {
        for (const auto in : ins)
        {
            if (data.at(in))
            {
                return 1;
            }
        }
        return 0;
    }

    std::uint32_t PropositionNode::evaluate(const std::vector<std::uint32_t> data) const
    {
        return 0;
    }

    std::uint32_t PreTransitionNode::evaluate(const std::vector<std::uint32_t> data) const
    {
        return 0;
    }

    std::uint32_t PostTransitionNode::evaluate(const std::vector<std::uint32_t> data) const
    {
        return 0;
    }

    NotNode::NotNode(std::uint32_t id, std::uint32_t in, std::vector<uint32_t> outs) :
        Node {id},
        in {in},
        outs {outs}
    {}

    std::uint32_t NotNode::evaluate(const std::vector<std::uint32_t> data) const
    {
        return data.at(in) ? 0 : 1;
    }

    ConstantNode::ConstantNode(std::uint32_t id, std::uint32_t value) :
        Node {id},
        value {value}
    {}

    std::uint32_t ConstantNode::evaluate(const std::vector<std::uint32_t> data) const
    {
        return value;
    }
};
