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

    bool AndNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        for (const auto in : ins)
        {
            if (!state.get_proposition(in))
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

    bool OrNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        for (const auto in : ins)
        {
            if (state.get_proposition(in))
            {
                return true;
            }
        }
        return false;
    }

    PropositionNode::PropositionNode(std::uint32_t id, std::string_view gdl) :
        Node {id},
        gdl {gdl}
    {}

    std::string_view PropositionNode::get_gdl() const
    {
        return gdl;
    }

    bool BasicPropositionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return false; // TODO
    }

    InputPropositionNode::InputPropositionNode(std::uint32_t id, std::string_view gdl) :
        PropositionNode {id, gdl}
    {}

    bool InputPropositionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        const auto id {get_id()};
        return actions.contains(id);
    }

    InitialPropositionNode::InitialPropositionNode(std::uint32_t id, std::string_view gdl) :
        PropositionNode {id, gdl}
    {}

    bool InitialPropositionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return state.get_is_initial_state();
    }

    PreTransitionNode::PreTransitionNode(std::uint32_t id, std::uint32_t in, std::uint32_t post_id) :
        Node {id},
        in {in},
        post_id {post_id}
    {}

    bool PreTransitionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return state.get_proposition(0); // TODO
    }

    PostTransitionNode::PostTransitionNode(std::uint32_t id, std::uint32_t pre_id, std::uint32_t out) :
        Node {id},
        pre_id {pre_id},
        out {out}
    {}

    bool PostTransitionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return state.get_proposition(pre_id);
    }

    NotNode::NotNode(std::uint32_t id, std::uint32_t in, std::vector<uint32_t> outs) :
        Node {id},
        in {in},
        outs {outs}
    {}

    bool NotNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return !state.get_proposition(in);
    }

    TrueNode::TrueNode(std::uint32_t id) :
        Node {id}
    {}

    bool TrueNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return true;
    }
};
