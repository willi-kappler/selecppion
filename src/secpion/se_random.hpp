/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines a pseudo random number generator

    Sources:
    https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/


*/

#ifndef FILE_SE_RANDOM_HPP_INCLUDED
#define FILE_SE_RANDOM_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <stdfloat>
#include <vector>

namespace secpion {
class SERandomLehmer64 {
    private:
        __uint128_t state;

    public:
        void seed(uint64_t s);
        uint64_t get_uint64();
        uint64_t get_uint64_n(uint64_t n);
        uint64_t get_uint64_n_slow(uint64_t n);
        uint32_t get_uint32_n(uint32_t n);
        uint16_t get_uint16_n(uint16_t n);
        uint8_t get_uint8_n(uint8_t n);

        std::float64_t get_float64_01();

        template <typename T>
        void shuffle(std::vector<T>& vec);

        // Constructor:
        SERandomLehmer64();
};

}

#endif // FILE_SE_RANDOM_HPP_INCLUDED