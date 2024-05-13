#pragma once

#include <cstdint>

namespace propnet
{
    // TODO
    template<typename T>
    class PersistentArray
    {
        public:
            PersistentArray(std::size_t size) {}

            T at(std::size_t idx) const
            {
                return T {};
            }
        private:
    };
};
