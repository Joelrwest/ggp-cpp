#pragma once

#include <cstdint>
#include <memory>
#include <limits>
#include <vector>
#include <iostream>

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

            bool get(std::uint32_t getting_id) const;
            void update(std::uint32_t updating_id, bool new_value);
            const std::vector<bool>& to_vector() const;
            bool get_is_initial() const;
            void set_not_is_initial();

            friend std::ostream& operator<<(std::ostream& os, const State& state);
        private:
            std::vector<bool> state;
            bool is_initial;
    };
}
