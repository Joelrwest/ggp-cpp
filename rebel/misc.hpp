#pragma once

#include <vector>
#include <cstdint>
#include <unordered_set>

namespace rebel::misc
{
    class CartesianProductGenerator
    {
        public:
            CartesianProductGenerator(const std::vector<std::vector<std::uint32_t>>& vecs);

            std::unordered_set<std::uint32_t> get();
            bool is_next() const;
            void operator++();
        private:
            struct ItPair
            {
                std::vector<std::uint32_t>::const_iterator curr_it;
                std::vector<std::uint32_t>::const_iterator end_it;
            };

            std::vector<ItPair> it_pairs;
    };
}
