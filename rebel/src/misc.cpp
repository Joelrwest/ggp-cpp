#include "misc.hpp"
#include <iostream>

namespace rebel::misc
{
    CartesianProductGenerator::CartesianProductGenerator(const std::vector<std::vector<std::uint32_t>>& vecs) :
        vec_its {},
        is_next {!vecs.empty()}
    {
        std::transform(
            vecs.begin(),
            vecs.end(),
            std::back_inserter(vec_its),
            [](const auto& vec)
            {
                return VecIt {
                    .it = vec.cbegin(),
                    .vec = vec,
                };
            }
        );
    }

    std::unordered_set<std::uint32_t> CartesianProductGenerator::get()
    {
        if (!get_is_next())
        {
            throw std::logic_error {"Tried to get next cartesian product but they've all been generated"};
        }

        std::unordered_set<std::uint32_t> product {};
        for (const auto& vec_it : vec_its)
        {
            product.insert(*vec_it.it);
        }

        return product;
    }

    bool CartesianProductGenerator::get_is_next() const
    {
        return is_next;
    }

    void CartesianProductGenerator::operator++()
    {
        if (!get_is_next())
        {
            throw std::logic_error {"Tried to increment to next cartesian product but they've all been generated"};
        }

        const auto front_begin_it {vec_its.front().vec.cbegin()};
        for (auto vec_it {vec_its.rbegin()}; vec_it != vec_its.rend(); ++vec_it)
        {
            ++vec_it->it;
            const auto is_end {vec_it->it == vec_it->vec.cend()};
            if (is_end)
            {
                const auto begin_it {vec_it->vec.cbegin()};
                vec_it->it = begin_it;
                is_next = begin_it != front_begin_it;
            }
            else
            {
                break;
            }
        }
    }
}
