#include "nodes.hpp"
#include <iostream>
namespace propnet
{
    Node::Node(std::uint32_t id) :
        id {id}
    {}

    std::uint32_t Node::get_id() const
    {
        return id;
    }

    AndNode::AndNode(std::uint32_t id, const std::vector<std::uint32_t>& ins) :
        Node {id},
        ins {ins}
    {}

    bool AndNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        for (const auto in : ins)
        {
            if (!state.get_prop(in))
            {
                return false;
            }
        }
        return true;
    }

    OrNode::OrNode(std::uint32_t id, const std::vector<std::uint32_t>& ins) :
        Node {id},
        ins {ins}
    {}

    bool OrNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        for (const auto in : ins)
        {
            if (state.get_prop(in))
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

    BasicPropositionNode::BasicPropositionNode(std::uint32_t id, std::string_view gdl, std::uint32_t in) :
        PropositionNode {id, gdl},
        in {in}
    {}

    bool BasicPropositionNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return state.get_prop(in);
    }

    InputPropositionNode::InputPropositionNode(std::uint32_t id, std::string_view gdl) :
        PropositionNode {id, gdl}
    {}

    bool InputPropositionNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        const auto id {get_id()};
        return inputs.contains(id);
    }

    InitialPropositionNode::InitialPropositionNode(std::uint32_t id, std::string_view gdl) :
        PropositionNode {id, gdl}
    {}

    bool InitialPropositionNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return state.get_is_initial_state();
    }

    PreTransitionNode::PreTransitionNode(std::uint32_t id, std::uint32_t in) :
        Node {id},
        in {in}
    {}

    bool PreTransitionNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return state.get_prop(in);
    }

    PostTransitionNode::PostTransitionNode(std::uint32_t id, std::uint32_t pre_id) :
        Node {id},
        pre_id {pre_id}
    {}

    bool PostTransitionNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return state.get_prop(pre_id);
    }

    NotNode::NotNode(std::uint32_t id, std::uint32_t in) :
        Node {id},
        in {in}
    {}

    bool NotNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return !state.get_prop(in);
    }

    TrueNode::TrueNode(std::uint32_t id) :
        Node {id}
    {}

    bool TrueNode::eval(const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return true;
    }
};
