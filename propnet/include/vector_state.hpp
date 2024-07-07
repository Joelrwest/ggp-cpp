#pragma once

#include <cstdint>
#include <memory>
#include <limits>
#include <vector>
#include <functional>

namespace propnet
{
    class State
    {
        public:
            State();
            State(std::size_t size);
            State(const State& other);
            State(State&& other);

            State& operator=(const State& other);
            State& operator=(State&& other);
            bool operator==(const State& other) const;

            bool get(std::uint32_t getting_id) const;
            void update(std::uint32_t updating_id, bool new_value);
            bool get_is_initial() const;
            void set_not_is_initial();

            friend std::ostream& operator<<(std::ostream& os, const State& state);
            friend class std::hash<propnet::State>;
        private:
            std::vector<bool> state;
            bool is_initial;
    };
}

namespace std
{
    template<>
    class hash<propnet::State>
    {
        public:
            std::size_t operator()(const propnet::State& state) const noexcept;
    };
}
