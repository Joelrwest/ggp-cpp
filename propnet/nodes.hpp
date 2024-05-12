#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace propnet
{
    class Node
    {
        public:
            Node(std::uint32_t id);
            std::uint32_t get_id() const;
            virtual std::uint32_t evaluate(const std::vector<std::uint32_t> data) const = 0;
        private:
            std::uint32_t id;
    };

    class AndNode : public Node
    {
        public:
            AndNode(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<uint32_t> outs);
            std::uint32_t evaluate(const std::vector<std::uint32_t> data) const override;
        private:
            std::vector<std::uint32_t> ins;
            std::vector<uint32_t> outs;
    };

    class OrNode : public Node
    {
        public:
            OrNode(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<uint32_t> outs);
            std::uint32_t evaluate(const std::vector<std::uint32_t> data) const override;
        private:
            std::vector<std::uint32_t> ins;
            std::vector<uint32_t> outs;
    };

    class PropositionNode : public Node
    {
        public:
            std::uint32_t evaluate(const std::vector<std::uint32_t> data) const override;
        private:
    };

    class PreTransitionNode : public Node
    {
        public:
            std::uint32_t evaluate(const std::vector<std::uint32_t> data) const override;
        private:
    };

    class PostTransitionNode : public Node
    {
        public:
            std::uint32_t evaluate(const std::vector<std::uint32_t> data) const override;
        private:
    };

    class NotNode : public Node
    {
        public:
            NotNode(std::uint32_t id, std::uint32_t in, std::vector<uint32_t> outs);
            std::uint32_t evaluate(const std::vector<std::uint32_t> data) const override;
        private:
            std::uint32_t in;
            std::vector<uint32_t> outs;
    };

    class ConstantNode : public Node
    {
        public:
            ConstantNode(std::uint32_t id, std::uint32_t value);
            std::uint32_t evaluate(const std::vector<std::uint32_t> data) const override;
        private:
            std::uint32_t value;
    };
};
