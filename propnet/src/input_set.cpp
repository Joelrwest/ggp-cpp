#include "../include/input_set.hpp"

namespace propnet
{
InputSet::InputSet() : inputs{}
{
}

InputSet::InputSet(PropId input) : inputs{input}
{
}

InputSet::InputSet(const std::unordered_set<PropId> &inputs) : inputs{inputs}
{
}

InputSet::InputSet(std::unordered_set<PropId> &&inputs) : inputs{std::move(inputs)}
{
}

InputSet::InputSet(std::initializer_list<PropId> &&inputs) : inputs{std::move(inputs)}
{
}

bool InputSet::contains(PropId input) const
{
    return inputs.contains(input);
}

void InputSet::add(PropId input)
{
    if (const auto [_, was_inserted]{inputs.insert(input)}; !was_inserted)
    {
        throw std::logic_error{"Inserted an input that was already present"};
    }
}

std::ostream &operator<<(std::ostream &os, const InputSet &inputs)
{
    os << '{';
    if (!inputs.inputs.empty())
    {
        os << ' ';
    }

    const auto end{inputs.inputs.end()};
    for (auto it{inputs.inputs.begin()}; it != end;)
    {
        os << *it;

        ++it;
        if (it != end)
        {
            os << ',';
        }
        os << ' ';
    }
    os << '}';

    return os;
}
} // namespace propnet

namespace std
{
std::size_t hash<propnet::InputSet>::operator()(const propnet::InputSet &inputs) const noexcept
{
    std::hash<propnet::PropId> input_hasher{};
    std::size_t result{0};
    for (const auto input : inputs.inputs)
    {
        result += input_hasher(input);
    }

    return result;
}
} // namespace std
