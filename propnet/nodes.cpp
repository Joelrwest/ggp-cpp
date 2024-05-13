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

    AndNode::AndNode(std::uint32_t id, std::vector<std::uint32_t> ins) :
        Node {id},
        ins {ins}
    {}

    bool AndNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        for (const auto in : ins)
        {
            if (!state.eval_prop(in))
            {
                return false;
            }
        }
        return true;
    }

    std::vector<std::uint32_t> AndNode::get_ins() const
    {
        return std::vector<std::uint32_t> {ins};
    }

    OrNode::OrNode(std::uint32_t id, std::vector<std::uint32_t> ins) :
        Node {id},
        ins {ins}
    {}

    bool OrNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        for (const auto in : ins)
        {
            if (state.eval_prop(in))
            {
                return true;
            }
        }
        return false;
    }

    std::vector<std::uint32_t> OrNode::get_ins() const
    {
        return std::vector<std::uint32_t> {ins};
    }

    PropositionNode::PropositionNode(std::uint32_t id, std::string_view gdl) :
        Node {id},
        gdl {gdl}
    {}

    std::string_view PropositionNode::get_gdl() const
    {
        return gdl;
    }

    BasicPropositionNode::BasicPropositionNode(std::uint32_t id, std::string_view gdl, std::uint32_t in) :
        PropositionNode {id, gdl},
        in {in}
    {}

    bool BasicPropositionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return state.eval_prop(in);
    }

    std::vector<std::uint32_t> BasicPropositionNode::get_ins() const
    {
        return std::vector<std::uint32_t> {in};
    }

    InputPropositionNode::InputPropositionNode(std::uint32_t id, std::string_view gdl) :
        PropositionNode {id, gdl}
    {}

    bool InputPropositionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        const auto id {get_id()};
        return actions.contains(id);
    }

    std::vector<std::uint32_t> InputPropositionNode::get_ins() const
    {
        return std::vector<std::uint32_t> {};
    }

    InitialPropositionNode::InitialPropositionNode(std::uint32_t id, std::string_view gdl) :
        PropositionNode {id, gdl}
    {}

    bool InitialPropositionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return state.get_is_initial_state();
    }

    std::vector<std::uint32_t> InitialPropositionNode::get_ins() const
    {
        return std::vector<std::uint32_t> {};
    }

    PreTransitionNode::PreTransitionNode(std::uint32_t id, std::uint32_t in) :
        Node {id},
        in {in}
    {}

    bool PreTransitionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return state.eval_prop(in);
    }

    std::vector<std::uint32_t> PreTransitionNode::get_ins() const
    {
        return std::vector<std::uint32_t> {in};
    }

    PostTransitionNode::PostTransitionNode(std::uint32_t id, std::uint32_t pre_id) :
        Node {id},
        pre_id {pre_id}
    {}

    bool PostTransitionNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return state.eval_prop(pre_id);
    }

    std::vector<std::uint32_t> PostTransitionNode::get_ins() const
    {
        return std::vector<std::uint32_t> {pre_id};
    }

    NotNode::NotNode(std::uint32_t id, std::uint32_t in) :
        Node {id},
        in {in}
    {}

    bool NotNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return !state.eval_prop(in);
    }

    std::vector<std::uint32_t> NotNode::get_ins() const
    {
        return std::vector<std::uint32_t> {in};
    }

    TrueNode::TrueNode(std::uint32_t id) :
        Node {id}
    {}

    bool TrueNode::evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const
    {
        return true;
    }

    std::vector<std::uint32_t> TrueNode::get_ins() const
    {
        return std::vector<std::uint32_t> {};
    }
};
