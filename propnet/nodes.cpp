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

    bool AndNode::evaluate(const PersistentArray<bool>& data) const
    {
        for (const auto in : ins)
        {
            if (!data.at(in))
            {
                return false;
            }
        }
        return true;
    }

    OrNode::OrNode(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<uint32_t> outs) :
        Node {id},
        ins {ins},
        outs {outs}
    {}

    bool OrNode::evaluate(const PersistentArray<bool>& data) const
    {
        for (const auto in : ins)
        {
            if (data.at(in))
            {
                return true;
            }
        }
        return false;
    }

    bool PropositionNode::evaluate(const PersistentArray<bool>& data) const
    {
        // TODO
        return false;
    }

    PreTransitionNode::PreTransitionNode(std::uint32_t id, std::uint32_t in, std::uint32_t post_id) :
        Node {id},
        in {in},
        post_id {post_id}
    {}

    bool PreTransitionNode::evaluate(const PersistentArray<bool>& data) const
    {
        return data.at(0); // TODO
    }

    PostTransitionNode::PostTransitionNode(std::uint32_t id, std::uint32_t pre_id, std::uint32_t out) :
        Node {id},
        pre_id {pre_id},
        out {out}
    {}

    bool PostTransitionNode::evaluate(const PersistentArray<bool>& data) const
    {
        return data.at(pre_id);
    }

    NotNode::NotNode(std::uint32_t id, std::uint32_t in, std::vector<uint32_t> outs) :
        Node {id},
        in {in},
        outs {outs}
    {}

    bool NotNode::evaluate(const PersistentArray<bool>& data) const
    {
        return !data.at(in);
    }

    ConstantNode::ConstantNode(std::uint32_t id, bool value) :
        Node {id},
        value {value}
    {}

    bool ConstantNode::evaluate(const PersistentArray<bool>& data) const
    {
        return value;
    }
};
