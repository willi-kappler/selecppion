/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines a pseudo random number generator
*/

// STD includes:
// #include <chrono>
#include <random>
#include <algorithm>

// Local includes:
#include "se_random.hpp"

namespace secpion {
    void SERandomLehmer64::seed(uint64_t s) {
        // Prevention: Most PRNGs fail if state is 0
        if (s == 0) s = 0xDEADC0DE;

        s = (s ^ (s >> 30)) * 0xbf58476d1ce4e5b9ULL;
        s = (s ^ (s >> 27)) * 0x94d049bb133111ebULL;
        state = s ^ (s >> 31);
    }

    uint64_t SERandomLehmer64::get_uint64() {
        state *= 0xda942042e4dd58b5;
        return state >> 64;
    }

    uint64_t SERandomLehmer64::get_uint64_n(uint64_t n) {
        // We want a value in [0, n).
        if (n == 0) return 0;

        // This is Lemire's nearly-divisionless algorithm.
        __uint128_t product = (__uint128_t)get_uint64() * n;
        uint64_t low_part = (uint64_t)product;

        if (low_part < n) {
            uint64_t threshold = -n % n; // This division only happens rarely
            while (low_part < threshold) {
                product = (__uint128_t)get_uint64() * n;
                low_part = (uint64_t)product;
            }
        }

        return (uint64_t)(product >> 64);
    }

    uint64_t SERandomLehmer64::get_uint64_n_slow(uint64_t n) {
        if (n == 0) return 0;
        return get_uint64() % (n + 1);
    }

    uint32_t SERandomLehmer64::get_uint32_n(uint32_t n) {
        return get_uint64_n(n);
    }

    uint16_t SERandomLehmer64::get_uint16_n(uint16_t n) {
        return get_uint64_n(n);
    }

    uint8_t SERandomLehmer64::get_uint8_n(uint8_t n) {
        return get_uint64_n(n);
    }

    std::float64_t SERandomLehmer64::get_float64_01() {
        return (get_uint64() >> 11) * (1.0 / 9007199254740992.0);
    }

    template <typename T>
    void SERandomLehmer64::shuffle(std::vector<T>& vec) {
        if (vec.empty()) return;

        // We iterate backwards from the last element to the second element
        for (size_t i = vec.size() - 1; i > 0; --i) {
            
            // Pick a random index j from 0 to i (inclusive)
            // We use your 'range' method here!
            size_t j = static_cast<size_t>(get_uint64_n(i));
            
            // Swap the elements
            std::swap(vec[i], vec[j]);
        }
    }


    // Constructor
    SERandomLehmer64::SERandomLehmer64():
        state()
    {
        std::random_device rd;
        state = (static_cast<uint64_t>(rd()) << 32) | rd();
    }
}

/*

uint64_t wyhash64_x; 


uint64_t wyhash64() {
  wyhash64_x += 0x60bee2bee120fc15;
  __uint128_t tmp;
  tmp = (__uint128_t) wyhash64_x * 0xa3b195354a39b70d;
  uint64_t m1 = (tmp >> 64) ^ tmp;
  tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
  uint64_t m2 = (tmp >> 64) ^ tmp;
  return m2;
}

*/

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