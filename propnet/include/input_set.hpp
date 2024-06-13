#pragma once

#include <cstdint>
#include <functional>
#include <unordered_set>

namespace propnet
{
    class InputSet
    {
        public:
            InputSet();
            InputSet(std::uint32_t input);
            InputSet(const InputSet& inputs) = default;
            InputSet(InputSet&& inputs) = default;
            InputSet(const std::unordered_set<std::uint32_t>& inputs);
            InputSet(std::unordered_set<std::uint32_t>&& inputs);

            InputSet& operator=(const InputSet& other) = default;
            InputSet& operator=(InputSet&& other) = default;
            bool operator==(const InputSet& other) const = default;

            bool contains(std::uint32_t input) const;
            void add(std::uint32_t input);

            friend class std::hash<InputSet>;
        private:
            std::unordered_set<std::uint32_t> inputs;
    };
}

namespace std
{
    template<>
    class hash<propnet::InputSet>
    {
        public:
            std::size_t operator()(const propnet::InputSet& state) const noexcept;
    };
}
