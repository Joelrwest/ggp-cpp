#include "../include/misc.hpp"

namespace player::misc
{
TimeOption::TimeOption() : time_limit{std::nullopt}
{
}

std::function<bool()> TimeOption::get_function() const
{
    if (time_limit.has_value())
    {
        return [end_time = std::chrono::system_clock::now() + *time_limit]() {
            return std::chrono::system_clock::now() < end_time;
        };
    }
    else
    {
        return always_time_remaining_function;
    }
}

IterationLimitOption::IterationLimitOption() : iteration_limit{std::numeric_limits<decltype(iteration_limit)>::max()}
{
}

IterationLimitOption::IterationLimitOption(std::size_t iteration_limit) : iteration_limit{iteration_limit}
{
}

void IterationLimitOption::add(std::size_t iteration_limit)
{
    this->iteration_limit = iteration_limit;
}

std::size_t IterationLimitOption::get() const
{
    return iteration_limit;
}
} // namespace player::misc
