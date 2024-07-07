#pragma once

#include <cache.hpp>

#include <vector>
#include <cstdint>
#include <unordered_set>
#include <random>

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

    template<typename T>
    const T::value_type& sample_random(const T& population)
    {
        static std::mt19937 random_engine {std::random_device {}()};
        static std::uniform_int_distribution<> distribution (0, population.size() - 1);
        const auto idx {distribution(random_engine)};
        return population[idx];
    }

    template<typename T>
    T sample_policy(const std::unordered_map<T, double>& policy)
    {
        static std::mt19937 random_engine {std::random_device {}()};
        static std::uniform_real_distribution<double> distribution (0.0, 1.0);
        const auto choice {distribution(random_engine)};
        double accumulation {0.0};
        for (const auto& [key, probability] : policy)
        {
            accumulation += probability;
            if (choice < accumulation)
            {
                return key;
            }
        }

        throw std::runtime_error {"Policy did not sum to 1.0"};
    }

    // Unfortunately cannot be reused from the policy code...
    template<typename T, typename U>
    T sample_counts(const std::unordered_map<T, U>& counts, U total_count)
    {
        static std::mt19937 random_engine {std::random_device {}()};
        static std::uniform_int_distribution<U> distribution (U {0}, total_count);
        const auto choice {distribution(random_engine)};
        U accumulation {0};
        for (const auto& [key, count] : counts)
        {
            accumulation += count;
            if (choice < accumulation)
            {
                return key;
            }
        }

        throw std::logic_error {"Should never occur"};
    }

    template<typename T, typename U>
    T sample_counts(const std::unordered_map<T, U>& counts)
    {
        const auto total_count {std::accumulate(
            counts.begin(),
            counts.end(),
            U {0},
            [](const auto accumulation, const auto& pair)
            {
                return accumulation + pair.second;
            }
        )};

        return sample_counts(counts, total_count);
    }

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
