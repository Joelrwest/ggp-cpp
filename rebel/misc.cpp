#include "misc.hpp"
#include <iostream>

namespace rebel::misc
{
    CartesianProductGenerator::CartesianProductGenerator(const std::vector<std::vector<std::uint32_t>>& vecs) :
        vec_its {}
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
        std::unordered_set<std::uint32_t> product {};
        for (const auto& vec_it : vec_its)
        {
            // TODO: Remove
            if (vec_it.it == vec_it.vec.cend())
            {
                std::cerr << "Something is terribly wrong\n";
                for (const auto& debug_it_pair : vec_its)
                {
                    std::cerr << std::distance(debug_it_pair.it, debug_it_pair.vec.cend()) << '\n';
                }
                throw std::logic_error {"Something is terribly wrong"};
            }
            product.insert(*vec_it.it);
        }

        return product;
    }

    bool CartesianProductGenerator::is_next() const
    {
        if (vec_its.empty())
        {
            return false;
        }

        const auto& first_it_pair {vec_its.front()};
        return first_it_pair.it != first_it_pair.vec.cend();
    }

    void CartesianProductGenerator::operator++()
    {
        for (auto vec_it {vec_its.rbegin()}; vec_it != vec_its.rend(); ++vec_it)
        {
            ++vec_it->it;
            const auto is_end {vec_it->it == vec_it->vec.cend()};
            if (is_end)
            {
                vec_it->it = vec_it->vec.cbegin();
            }
            else
            {
                break;
            }
        }
    }
}
