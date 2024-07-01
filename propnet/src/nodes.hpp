#pragma once

#include "../include/vector_state.hpp"
#include "../include/input_set.hpp"

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_set>

namespace propnet
{
    class Node
    {
        public:
            Node(std::uint32_t id);

            std::uint32_t get_id() const;
            virtual bool eval(const State& state, const InputSet& inputs) const = 0;
        private:
            std::uint32_t id;
    };

    class AndNode : public Node
    {
        public:
            AndNode(std::uint32_t id, const std::vector<std::uint32_t>& ins);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::vector<std::uint32_t> ins;
    };

    class OrNode : public Node
    {
        public:
            OrNode(std::uint32_t id, const std::vector<std::uint32_t>& ins);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::vector<std::uint32_t> ins;
    };

    class PropositionNode : public Node
    {
        public:
            PropositionNode(std::uint32_t id, std::string_view display);

            std::string_view get_display() const;
        private:
            std::string display;
    };

    class BasicPropositionNode : public PropositionNode
    {
        public:
            BasicPropositionNode(std::uint32_t id, std::string_view display, std::uint32_t in);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::uint32_t in;
    };

    class InputPropositionNode : public PropositionNode
    {
        public:
            InputPropositionNode(std::uint32_t id, std::string_view display);
            bool eval(const State& state, const InputSet& inputs) const override;
    };

    class InitialPropositionNode : public PropositionNode
    {
        public:
            InitialPropositionNode(std::uint32_t id, std::string_view display);
            bool eval(const State& state, const InputSet& inputs) const override;
    };

    class PreTransitionNode : public Node
    {
        public:
            PreTransitionNode(std::uint32_t id, std::uint32_t in);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::uint32_t in;
    };

    class PostTransitionNode : public Node
    {
        public:
            PostTransitionNode(std::uint32_t id, std::uint32_t pre_id);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::uint32_t pre_id;
    };

    class NotNode : public Node
    {
        public:
            NotNode(std::uint32_t id, std::uint32_t in);

            bool eval(const State& state, const InputSet& inputs) const override;
        private:
            std::uint32_t in;
    };

    class TrueNode : public Node
    {
        public:
            TrueNode(std::uint32_t id);

            bool eval(const State& state, const InputSet& inputs) const override;
    };
}
