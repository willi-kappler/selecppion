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
SEAlgorithmLehmer64::SEAlgorithmLehmer64():
    state(0x9e3779b97f4a7c15) {}

void SEAlgorithmLehmer64::seed(uint64_t s) {
    state = state ^ s;
}

uint64_t SEAlgorithmLehmer64::next_u64() {
    state *= 0xda942042e4dd58b5;
    return state >> 64;
}

template <SERandomAlgorithm T>
void SERandomGenerator<T>::seed(uint64_t s) {
    // Prevention: Most PRNGs fail if state is 0
    if (s == 0) s = 0xDEADC0DE;

    s = (s ^ (s >> 30)) * 0xbf58476d1ce4e5b9ULL;
    s = (s ^ (s >> 27)) * 0x94d049bb133111ebULL;
    s = s ^ (s >> 31);

    random_algorithm.seed(s);
}

template <SERandomAlgorithm T>
void SERandomGenerator<T>::seed() {
    std::random_device rd;
    uint64_t s = (static_cast<uint64_t>(rd()) << 32) | rd();
    random_algorithm.seed(s);
}

template <SERandomAlgorithm T>
uint64_t SERandomGenerator<T>::get_uint64(uint64_t n) {
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

template <SERandomAlgorithm T>
uint32_t SERandomGenerator<T>::get_uint32(uint32_t n) {
    return get_uint64(n);
}

template <SERandomAlgorithm T>
uint16_t SERandomGenerator<T>::get_uint16(uint16_t n) {
    return get_uint64(n);
}

template <SERandomAlgorithm T>
uint8_t SERandomGenerator<T>::get_uint8(uint8_t n) {
    return get_uint64(n);
}

template <SERandomAlgorithm T>
size_t SERandomGenerator<T>::get_size_t(size_t n) {
    return get_uint64(n);
}

template <SERandomAlgorithm T>
std::float64_t SERandomGenerator<T>::get_float64() {
    const uint64_t v = (random_algorithm.next_u64() >> 11);
    return v * (1.0 / 9007199254740992.0);
}

template <SERandomAlgorithm T>
template <typename U>
void SERandomGenerator<T>::shuffle(std::vector<U>& v) {
    if (v.empty()) return;

    // We iterate backwards from the last element to the second element (Knuth)
    for (size_t i = v.size() - 1; i > 0; --i) {

        // Pick a random index j from 0 to i (inclusive)
        size_t j = static_cast<size_t>(random_algorithm.get_uint64(i));

        // Swap the elements
        std::swap(v[i], v[j]);
    }
}

template <SERandomAlgorithm T>
template <typename U>
U SERandomGenerator<T>::choice(std::vector<U>& v) {
    U result;
    size_t len = v.size();

    if (len > 0) {
        size_t i = get_size_t(len);
        result = v[i];
    }

    return result;
}

template <SERandomAlgorithm T>
SERandomGenerator<T>::SERandomGenerator():
    random_algorithm() {}
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