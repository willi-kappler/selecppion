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
#include <concepts>

namespace secpion {
template <typename T>
concept SERandomAlgorithm = requires(T rnd_algo, uint64_t s) {
    { rnd_algo.next_u64() } -> std::convertible_to<uint64_t>;
    { rnd_algo.seed(s) };
};

class SEAlgorithmLehmer64 {
    private:
        __uint128_t state;

    public:
        void seed(uint64_t);
        uint64_t next_u64();
        SEAlgorithmLehmer64();
};

template <SERandomAlgorithm T>
class SERandomGenerator {
    private:
        T random_algorithm;

    public:
        void seed(uint64_t);
        void seed();

        uint64_t get_uint64(uint64_t);
        uint32_t get_uint32(uint32_t);
        uint16_t get_uint16(uint16_t);
        uint8_t get_uint8(uint8_t) ;
        size_t get_size_t(size_t);

        std::float64_t get_float64();

        template <typename U>
        void shuffle(std::vector<U>&);

        template <typename U>
        U choice(std::vector<U>&);

        // Constructor:
        SERandomGenerator();
};

}

#endif // FILE_SE_RANDOM_HPP_INCLUDED