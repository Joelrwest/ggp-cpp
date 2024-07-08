#pragma once

#include "../include/vector_state.hpp"
#include "../include/input_set.hpp"
#include "../include/types.hpp"

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_set>

namespace propnet
{
    class Node
    {
        public:
            Node(PropId id);

            PropId get_id() const;
            virtual bool eval(const State& state, const InputSet& inputs) const = 0;
        private:
            PropId id;
    };

    class AndNode : public Node
    {
        public:
            AndNode(PropId id, const std::vector<PropId>& ins);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::vector<PropId> ins;
    };

    class OrNode : public Node
    {
        public:
            OrNode(PropId id, const std::vector<PropId>& ins);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::vector<PropId> ins;
    };

    class PropositionNode : public Node
    {
        public:
            PropositionNode(PropId id, std::string_view display);

            std::string_view get_display() const;
        private:
            std::string display;
    };

    class BasicPropositionNode : public PropositionNode
    {
        public:
            BasicPropositionNode(PropId id, std::string_view display, PropId in);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            PropId in;
    };

    class InputPropositionNode : public PropositionNode
    {
        public:
            InputPropositionNode(PropId id, std::string_view display);
            bool eval(const State& state, const InputSet& inputs) const override;
    };

    class InitialPropositionNode : public PropositionNode
    {
        public:
            InitialPropositionNode(PropId id, std::string_view display);
            bool eval(const State& state, const InputSet& inputs) const override;
    };

    class PreTransitionNode : public Node
    {
        public:
            PreTransitionNode(PropId id, PropId in);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            PropId in;
    };

    class PostTransitionNode : public Node
    {
        public:
            PostTransitionNode(PropId id, PropId pre_id);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            PropId pre_id;
    };

    class NotNode : public Node
    {
        public:
            NotNode(PropId id, PropId in);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            PropId in;
    };

    class TrueNode : public Node
    {
        public:
            TrueNode(PropId id);

            bool eval(const State& state, const InputSet& inputs) const override;
    };
}
