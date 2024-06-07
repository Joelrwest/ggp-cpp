#pragma once

#include <cache.hpp>

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
            bool get_is_next() const;
            void operator++();
        private:
            struct VecIt
            {
                std::vector<std::uint32_t>::const_iterator it;
                const std::vector<std::uint32_t>& vec;
            };

            std::vector<VecIt> vec_its;
            bool is_next;
    };

    /*
    Hacky way to get access to the clear method,
    since it's protected in the implementation...
    */
    template<typename KeyT, typename ValueT, template<typename> typename PolicyT>
    using UnderlyingCache = caches::fixed_sized_cache<KeyT, ValueT, PolicyT>;

    template<typename KeyT, typename ValueT, template<typename> typename PolicyT, std::size_t SIZE>
    class Cache : public UnderlyingCache<KeyT, ValueT, PolicyT>
    {
        public:
            Cache() :
                UnderlyingCache<KeyT, ValueT, PolicyT> {SIZE}
            {}

            void clear()
            {
                this->Clear();
            }
    };
}
