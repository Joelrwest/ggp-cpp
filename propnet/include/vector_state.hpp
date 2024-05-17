#pragma once

#include <cstdint>
#include <memory>
#include <limits>
#include <vector>

namespace propnet
{
    class State
    {
        public:
            State(std::size_t size);
            State(const State& other);

            bool get(std::uint32_t getting_id) const;
            void update(std::uint32_t updating_id, bool new_value);
            const std::vector<bool>& to_vector() const;
            bool get_is_initial() const;
            void set_not_is_initial();
        private:
            std::vector<bool> state;
            bool is_initial;
    };
};
