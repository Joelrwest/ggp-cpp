#pragma once

#include "types.hpp"

#include <cache.hpp>

#include <cstdint>
#include <random>
#include <span>
#include <unordered_set>
#include <vector>

namespace player::misc
{
template <typename T> class LazyCartesianProductGenerator
{
  public:
    // TODO: Idk why but it doesn't work as LazyCartesianProductGenerator(std::span<const std::vector<T>> vecs)
    LazyCartesianProductGenerator(const std::vector<std::vector<T>> &vecs);

    std::unordered_set<T> get();
    bool get_is_next() const;
    void operator++();

  private:
    struct ItPair
    {
        std::span<const T> span;
        decltype(span)::iterator it;

        ItPair(std::span<const T> span);
    };

    std::vector<ItPair> it_pairs;
    bool is_next;
};

template <typename T> const T::value_type &sample_random(const T &population);
template <typename T> T sample_policy(const std::unordered_map<T, Probability> &policy);
template <typename T> T sample_policy(const std::unordered_map<T, Probability> &policy, Probability policy_sum);

// Unfortunately cannot be reused from the policy code...
template <typename T, typename U> T sample_counts(const std::unordered_map<T, U> &counts, U total_count);
template <typename T, typename U> T sample_counts(const std::unordered_map<T, U> &counts);
template <typename T> static T make_zeroed_map(std::span<const propnet::PropId> legal_inputs);

/*
Hacky way to get access to the clear method,
since it's protected in the implementation...
*/
template <typename KeyT, typename ValueT, template <typename> typename PolicyT>
using UnderlyingCache = caches::fixed_sized_cache<KeyT, ValueT, PolicyT>;

template <typename KeyT, typename ValueT, template <typename> typename PolicyT, std::size_t SIZE>
class Cache : public UnderlyingCache<KeyT, ValueT, PolicyT>
{
  public:
    Cache();

    void clear();
};

template <typename T>
LazyCartesianProductGenerator<T>::ItPair::ItPair(std::span<const T> span) : span{span}, it{span.begin()}
{
}

template <typename T>
LazyCartesianProductGenerator<T>::LazyCartesianProductGenerator(const std::vector<std::vector<T>> &vecs)
    : it_pairs{}, is_next{!vecs.empty()}
{
    std::transform(vecs.begin(), vecs.end(), std::back_inserter(it_pairs), [](const auto &vec) { return ItPair{vec}; });
}

template <typename T> std::unordered_set<T> LazyCartesianProductGenerator<T>::get()
{
    if (!get_is_next())
    {
        throw std::logic_error{"Tried to get next cartesian product but they've all been generated"};
    }

    std::unordered_set<T> product{};
    for (const auto &it_pair : it_pairs)
    {
        product.insert(*it_pair.it);
    }

    return product;
}

template <typename T> bool LazyCartesianProductGenerator<T>::get_is_next() const
{
    return is_next;
}

template <typename T> void LazyCartesianProductGenerator<T>::operator++()
{
    if (!get_is_next())
    {
        throw std::logic_error{"Tried to increment to next cartesian product but they've all been generated"};
    }

    const auto front_begin_it{it_pairs.front().span.begin()};
    for (auto it_pair{it_pairs.rbegin()}; it_pair != it_pairs.rend(); ++it_pair)
    {
        ++it_pair->it;
        const auto is_end{it_pair->it == it_pair->span.end()};
        if (is_end)
        {
            const auto begin_it{it_pair->span.begin()};
            it_pair->it = begin_it;
            is_next = begin_it != front_begin_it;
        }
        else
        {
            break;
        }
    }
}

template <typename T> const T::value_type &sample_random(const T &population)
{
    static std::mt19937 random_engine{std::random_device{}()};
    static std::uniform_int_distribution<> distribution(0, population.size() - 1);
    const auto idx{distribution(random_engine)};
    return population[idx];
}

template <typename T> T sample_policy(const std::unordered_map<T, Probability> &policy)
{
    return sample_policy(policy, Probability {1.0});
}

template <typename T> T sample_policy(const std::unordered_map<T, Probability> &policy, Probability policy_sum)
{
    static std::mt19937 random_engine{std::random_device{}()};
    static std::uniform_real_distribution<Probability> distribution(0.0, policy_sum);
    const auto choice{distribution(random_engine)};
    Probability accumulation{0.0};
    for (const auto &[key, probability] : policy)
    {
        accumulation += probability;
        if (choice < accumulation)
        {
            return key;
        }
    }

    throw std::runtime_error{"Policy did not sum to policy_sum"};
}

template <typename T, typename U> T sample_counts(const std::unordered_map<T, U> &counts, U total_count)
{
    static std::mt19937 random_engine{std::random_device{}()};
    static std::uniform_int_distribution<U> distribution(U{0}, total_count);
    const auto choice{distribution(random_engine)};
    U accumulation{0};
    for (const auto &[key, count] : counts)
    {
        accumulation += count;
        if (choice < accumulation)
        {
            return key;
        }
    }

    throw std::logic_error{"Error in sample_counts where the sampled number was more than the sum of the counts"};
}

template <typename T, typename U> T sample_counts(const std::unordered_map<T, U> &counts)
{
    const auto total_count{
        std::accumulate(counts.begin(), counts.end(), U{0},
                        [](const auto accumulation, const auto &pair) { return accumulation + pair.second; })};

    return sample_counts(counts, total_count);
}

template <typename T> T make_zeroed_map(std::span<const propnet::PropId> legal_inputs)
{
    T map{};
    for (const auto legal_input : legal_inputs)
    {
        map.emplace(legal_input, 0.0);
    }

    return map;
}

template <typename KeyT, typename ValueT, template <typename> typename PolicyT, std::size_t SIZE>
Cache<KeyT, ValueT, PolicyT, SIZE>::Cache() : UnderlyingCache<KeyT, ValueT, PolicyT>{SIZE}
{
}

template <typename KeyT, typename ValueT, template <typename> typename PolicyT, std::size_t SIZE>
void Cache<KeyT, ValueT, PolicyT, SIZE>::clear()
{
    this->Clear();
}
} // namespace player::misc
