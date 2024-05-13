#pragma once

#include "state.hpp"

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
            virtual bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const = 0;
        private:
            std::uint32_t id;
    };

    class AndNode : public Node
    {
        public:
            AndNode(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<std::uint32_t> outs);
            bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const override;
        private:
            std::vector<std::uint32_t> ins;
            std::vector<std::uint32_t> outs;
    };

    class OrNode : public Node
    {
        public:
            OrNode(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<std::uint32_t> outs);
            bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const override;
        private:
            std::vector<std::uint32_t> ins;
            std::vector<std::uint32_t> outs;
    };

    class PropositionNode : public Node
    {
        public:
            PropositionNode(std::uint32_t id, std::string_view gdl);
            std::string_view get_gdl() const;
        private:
            std::string gdl;
    };

    class BasicPropositionNode : public PropositionNode
    {
        public:
            BasicPropositionNode(std::uint32_t id, std::string_view gdl);
            bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const override;
        private:
    };

    class PreTransitionNode : public Node
    {
        public:
            PreTransitionNode(std::uint32_t id, std::uint32_t in, std::uint32_t post_id);
            bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const override;
        private:
            std::uint32_t in;
            std::uint32_t post_id;
    };

    class PostTransitionNode : public Node
    {
        public:
            PostTransitionNode(std::uint32_t id, std::uint32_t pre_id, std::uint32_t out);
            bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const override;
        private:
            std::uint32_t pre_id;
            std::uint32_t out;
    };

    class NotNode : public Node
    {
        public:
            NotNode(std::uint32_t id, std::uint32_t in, std::vector<std::uint32_t> outs);
            bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const override;
        private:
            std::uint32_t in;
            std::vector<std::uint32_t> outs;
    };

    class ConstantNode : public Node
    {
        public:
            ConstantNode(std::uint32_t id, bool value);
            bool evaluate(const State& state, const std::unordered_set<std::uint32_t>& actions) const override;
        private:
            bool value;
    };
};
