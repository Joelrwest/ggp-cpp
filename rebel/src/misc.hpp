#pragma once

#include <cache.hpp>

#include <vector>
#include <cstdint>
#include <unordered_set>

namespace rebel::misc
{
    template<typename T>
    class LazyCartesianProductGenerator
    {
        private:
            struct VecIt
            {
                std::vector<T>::const_iterator it;
                const std::vector<T>& vec;
            };

            std::vector<VecIt> vec_its;
            bool is_next;
        public:
            LazyCartesianProductGenerator(const std::vector<std::vector<T>>& vecs) :
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

            std::unordered_set<T> get()
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

            bool get_is_next() const
            {
                return is_next;
            }

            void operator++()
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
