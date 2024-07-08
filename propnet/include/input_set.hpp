#pragma once

#include "types.hpp"

#include <cstdint>
#include <functional>
#include <unordered_set>

namespace propnet
{
    class InputSet
    {
        public:
            InputSet();
            InputSet(PropId input);
            InputSet(const InputSet& inputs) = default;
            InputSet(InputSet&& inputs) = default;
            InputSet(const std::unordered_set<PropId>& inputs);
            InputSet(std::unordered_set<PropId>&& inputs);

            InputSet& operator=(const InputSet& other) = default;
            InputSet& operator=(InputSet&& other) = default;
            bool operator==(const InputSet& other) const = default;

            bool contains(PropId input) const;
            void add(PropId input);

            friend class std::hash<InputSet>;
        private:
            std::unordered_set<PropId> inputs;
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
