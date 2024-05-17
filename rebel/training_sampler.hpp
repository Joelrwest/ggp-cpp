#include "include/rebel.hpp"

namespace rebel
{
    class TrainingSampler
    {
        public:
            TrainingSampler(const propnet::Propnet& propnet);

            void add_sees(const std::vector<bool>& sees);
        private:
            std::vector<std::vector<bool>> all_sees {};
            const propnet::Propnet& propnet;
    };
};
