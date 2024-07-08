#include "nodes.hpp"

namespace propnet
{
    Node::Node(PropId id) :
        id {id}
    {}

    PropId Node::get_id() const
    {
        return id;
    }

    AndNode::AndNode(PropId id, const std::vector<PropId>& ins) :
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

    OrNode::OrNode(PropId id, const std::vector<PropId>& ins) :
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

    PropositionNode::PropositionNode(PropId id, std::string_view display) :
        Node {id},
        display {display}
    {}

    std::string_view PropositionNode::get_display() const
    {
        return display;
    }

    BasicPropositionNode::BasicPropositionNode(PropId id, std::string_view display, PropId in) :
        PropositionNode {id, display},
        in {in}
    {}

    bool BasicPropositionNode::eval(const State& state, const InputSet&) const
    {
        return state.get(in);
    }

    InputPropositionNode::InputPropositionNode(PropId id, std::string_view display) :
        PropositionNode {id, display}
    {}

    bool InputPropositionNode::eval(const State&, const InputSet& inputs) const
    {
        const auto id {get_id()};
        return inputs.contains(id);
    }

    InitialPropositionNode::InitialPropositionNode(PropId id, std::string_view display) :
        PropositionNode {id, display}
    {}

    bool InitialPropositionNode::eval(const State& state, const InputSet&) const
    {
        return state.get_is_initial();
    }

    PreTransitionNode::PreTransitionNode(PropId id, PropId in) :
        Node {id},
        in {in}
    {}

    bool PreTransitionNode::eval(const State& state, const InputSet&) const
    {
        return state.get(in);
    }

    PostTransitionNode::PostTransitionNode(PropId id, PropId pre_id) :
        Node {id},
        pre_id {pre_id}
    {}

    bool PostTransitionNode::eval(const State& state, const InputSet&) const
    {
        return state.get(pre_id);
    }

    NotNode::NotNode(PropId id, PropId in) :
        Node {id},
        in {in}
    {}

    bool NotNode::eval(const State& state, const InputSet&) const
    {
        return !state.get(in);
    }

    TrueNode::TrueNode(PropId id) :
        Node {id}
    {}

    bool TrueNode::eval(const State&, const InputSet&) const
    {
        return true;
    }
}
