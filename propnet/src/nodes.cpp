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

    bool AndNode::eval(const State& state, const InputSet&) const
    {
        for (const auto in : ins)
        {
            if (!state.get(in))
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

    bool OrNode::eval(const State& state, const InputSet&) const
    {
        for (const auto in : ins)
        {
            if (state.get(in))
            {
                return true;
            }
        }
        return false;
    }

    PropositionNode::PropositionNode(std::uint32_t id, std::string_view display) :
        Node {id},
        display {display}
    {}

    std::string_view PropositionNode::get_display() const
    {
        return display;
    }

    BasicPropositionNode::BasicPropositionNode(std::uint32_t id, std::string_view display, std::uint32_t in) :
        PropositionNode {id, display},
        in {in}
    {}

    bool BasicPropositionNode::eval(const State& state, const InputSet&) const
    {
        return state.get(in);
    }

    InputPropositionNode::InputPropositionNode(std::uint32_t id, std::string_view display) :
        PropositionNode {id, display}
    {}

    bool InputPropositionNode::eval(const State&, const InputSet& inputs) const
    {
        const auto id {get_id()};
        return inputs.contains(id);
    }

    InitialPropositionNode::InitialPropositionNode(std::uint32_t id, std::string_view display) :
        PropositionNode {id, display}
    {}

    bool InitialPropositionNode::eval(const State& state, const InputSet&) const
    {
        return state.get_is_initial();
    }

    PreTransitionNode::PreTransitionNode(std::uint32_t id, std::uint32_t in) :
        Node {id},
        in {in}
    {}

    bool PreTransitionNode::eval(const State& state, const InputSet&) const
    {
        return state.get(in);
    }

    PostTransitionNode::PostTransitionNode(std::uint32_t id, std::uint32_t pre_id) :
        Node {id},
        pre_id {pre_id}
    {}

    bool PostTransitionNode::eval(const State& state, const InputSet&) const
    {
        return state.get(pre_id);
    }

    NotNode::NotNode(std::uint32_t id, std::uint32_t in) :
        Node {id},
        in {in}
    {}

    bool NotNode::eval(const State& state, const InputSet&) const
    {
        return !state.get(in);
    }

    TrueNode::TrueNode(std::uint32_t id) :
        Node {id}
    {}

    bool TrueNode::eval(const State&, const InputSet&) const
    {
        return true;
    }
}
