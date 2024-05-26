#include "misc.hpp"

namespace rebel::misc
{
    CartesianProductGenerator::CartesianProductGenerator(const std::vector<std::vector<std::uint32_t>>& vecs) :
        it_pairs {}
    {
        std::transform(
            vecs.begin(),
            vecs.end(),
            std::back_inserter(it_pairs),
            [](const auto& vec)
            {
                return ItPair {
                    .curr_it = vec.begin(),
                    .end_it = vec.end(),
                };
            }
        );
    }

    std::unordered_set<std::uint32_t> CartesianProductGenerator::get()
    {
        std::unordered_set<std::uint32_t> product {};
        for (const auto& it_pair : it_pairs)
        {
            product.insert(*it_pair.curr_it);
        }

        return product;
    }

    bool CartesianProductGenerator::is_next() const
    {
        if (it_pairs.empty())
        {
            return false;
        }

        const auto& first_it_pair {it_pairs.front()};
        return first_it_pair.curr_it != first_it_pair.end_it;
    }

    void CartesianProductGenerator::operator++()
    {
        auto prev_looped {true};
        for (auto it_pair {it_pairs.rbegin()}; it_pair != it_pairs.rend(); ++it_pair)
        {
            const auto next_it {prev_looped ? it_pair->curr_it + 1 : it_pair->curr_it};
            prev_looped = (it_pair->curr_it == it_pair->end_it);
            it_pair->curr_it = next_it;
        }
    }
}
