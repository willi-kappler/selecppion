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
#include <random>
#include <algorithm>

// Local includes:
#include "se_exceptions.hpp"

namespace secpion {
template <typename T>
concept SERandomAlgorithm = requires(T rnd_algo, uint64_t s) {
    { rnd_algo.next_u64() } -> std::convertible_to<uint64_t>;
    { rnd_algo.seed(s) };
};

// Lehmer64: https://en.wikipedia.org/wiki/Lehmer_random_number_generator
class SEAlgorithmLehmer64 {
    private:
        __uint128_t state;

    public:
        void seed(uint64_t s) {
            state = s;
        }

        uint64_t next_u64() {
            state *= 0xda942042e4dd58b5;
            return state >> 64;
        }

        SEAlgorithmLehmer64():
            state(0x9e3779b97f4a7c15) {}
};

// WyRand / WyHash: Wang Yi, https://github.com/wangyi-fudan/wyhash, unlicense
class SEAlgorithmWyRand {
    private:
        uint64_t state;

    public:
        void seed(uint64_t s) {
            state = s;
        }

        uint64_t next_u64() {
            state += 0x60bee2bee120fc15;
            __uint128_t tmp;
            tmp = (__uint128_t) state * 0xa3b195354a39b70d;
            uint64_t m1 = (tmp >> 64) ^ tmp;
            tmp = (__uint128_t) m1 * 0x1b03738712fad5c9;
            uint64_t m2 = (tmp >> 64) ^ tmp;
            return m2;
        }

        SEAlgorithmWyRand():
            state(0x9e3779b97f4a7c15) {}
};

// STD Linear Congruential
class SEAlgorithmSTD_LCG {
    std::independent_bits_engine<std::minstd_rand0, 64, uint64_t> engine;

    public:
        void seed(uint64_t s) {
            engine.seed(s);
        }

        uint64_t next_u64() {
            return engine();
        }
};

// STD Mersenne Twister
class SEAlgorithmSTD_Mersenne {
    std::mt19937_64 engine;

    public:
        void seed(uint64_t s) {
            engine.seed(s);
        }

        uint64_t next_u64() {
            return engine();
        }
};

// STD Subtract With Carry
class SEAlgorithmSTD_SWC {
    std::independent_bits_engine<std::ranlux48_base, 64, uint64_t> engine;

    public:
        void seed(uint64_t s) {
            engine.seed(s);
        }

        uint64_t next_u64() {
            return engine();
        }
};

template <SERandomAlgorithm T>
class SERandomGenerator {
    private:
        T random_algorithm;

    public:
        void seed(uint64_t s) {
            // Prevention: Most PRNGs fail if state is 0
            if (s == 0) s = 0xDEADC0DE;

            s = (s ^ (s >> 30)) * 0xbf58476d1ce4e5b9ULL;
            s = (s ^ (s >> 27)) * 0x94d049bb133111ebULL;
            s = s ^ (s >> 31);

            random_algorithm.seed(s);
        }

        void seed() {
            std::random_device rd;
            uint64_t s = (static_cast<uint64_t>(rd()) << 32) | rd();
            random_algorithm.seed(s);
        }

        uint64_t get_uint64(uint64_t n) {
            // We want a value in [0, n)
            if (n == 0) return 0;

            // This is Lemire's nearly-divisionless algorithm
            __uint128_t product = (__uint128_t) random_algorithm.next_u64() * n;

            uint64_t low_part = static_cast<uint64_t>(product);

            if (low_part < n) {
                // This division only happens rarely
                uint64_t threshold = -n % n;
                while (low_part < threshold) {
                    product = (__uint128_t) random_algorithm.next_u64() * n;
                    low_part = static_cast<uint64_t>(product);
                }
            }

            return static_cast<uint64_t>(product >> 64);
        }

        uint32_t get_uint32(uint32_t n) {
            return get_uint64(n);
        }

        uint16_t get_uint16(uint16_t n) {
            return get_uint64(n);
        }

        uint8_t get_uint8(uint8_t n) {
            return get_uint64(n);
        }

        size_t get_size_t(size_t n) {
            return get_uint64(n);
        }

        std::float64_t get_float64() {
            const uint64_t v = (random_algorithm.next_u64() >> 11);
            return v * (1.0 / 9007199254740992.0);
        }

        std::float64_t get_float64(std::float64_t start, std::float64_t end) {
            return (get_float64() * (end - start)) + start;
        }

        template <typename U>
        void shuffle(std::vector<U>& v) {
            if (v.size() < 2) return;

            // We iterate backwards from the last element to the second element (Knuth)
            for (size_t i = v.size() - 1; i > 0; --i) {

                // Pick a random index j from 0 to i (inclusive)
                size_t j = get_size_t(i + 1);

                // Swap the elements
                std::swap(v[i], v[j]);
            }
        }

        template <typename U>
        U choice(std::vector<U>& v) {
            size_t len = v.size();

            if (len == 0) {
                throw SERNGException("Called choice() with empty vector!");
            } else if (len == 1) {
                return v[0];
            } else {
                size_t i = get_size_t(len);
                return v[i];
            }
        }

        // Constructor:
        SERandomGenerator():
            random_algorithm() {}
};
}

#endif // FILE_SE_RANDOM_HPP_INCLUDED

/*

#include <chrono>
#include <cstdint>

uint64_t generateBetterSeed() {
    uint64_t time = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    // A simple "SplitMix" style hash to scramble the bits
    // This spreads the 'entropy' across all 64 bits
    time ^= (time >> 33);
    time *= 0xff51afd7ed558ccdULL;
    time ^= (time >> 33);
    time *= 0xc4ceb9fe1a85ec53ULL;
    time ^= (time >> 33);

    return time;
}

*/
