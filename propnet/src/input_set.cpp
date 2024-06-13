#include "../include/input_set.hpp"

namespace propnet
{
    InputSet::InputSet() :
        inputs {}
    {}

    InputSet::InputSet(std::uint32_t input) :
        inputs {input}
    {}

    InputSet::InputSet(const std::unordered_set<std::uint32_t>& inputs) :
        inputs {inputs}
    {}

    InputSet::InputSet(std::unordered_set<std::uint32_t>&& inputs) :
        inputs {inputs}
    {}

    bool InputSet::contains(std::uint32_t input) const
    {
        return inputs.contains(input);
    }

    void InputSet::add(std::uint32_t input)
    {
        inputs.insert(input);
    }
}

namespace std
{
    std::size_t hash<propnet::InputSet>::operator()(const propnet::InputSet& inputs) const noexcept
    {
        std::hash<std::uint32_t> input_hasher {};
        std::size_t result {0};
        for (const auto input : inputs.inputs)
        {
            result += input_hasher(input);
        }

        return result;
    }
}
