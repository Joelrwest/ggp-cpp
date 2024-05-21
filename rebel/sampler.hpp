#pragma once

#include "../propnet/include/vector_state.hpp"
#include "../propnet/include/propnet.hpp"

#include <concepts>

namespace rebel
{
    template<typename DerivedT>
    class Sampler
    {
        private:
            DerivedT& to_derived()
            {
                return static_cast<DerivedT&>(*this);
            }

            std::vector<std::vector<bool>> all_sees {};
        public:
            static constexpr auto SAMPLE_SIZE {10};

            const std::vector<std::vector<bool>>& get_all_sees() const
            {
                return all_sees;
            }

            void add_sees(const std::vector<bool>& sees)
            {
                all_sees.push_back(sees);
            }

            std::vector<propnet::State> sample()
            {
                return to_derived().sample();
            }

            void new_game()
            {
                to_derived().new_game();
            }
    };
}
