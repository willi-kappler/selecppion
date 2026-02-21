/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the random number generator classes.

    Run only configuration tests:
    xmake run -w ./ se_test [random]
*/

// STD includes:
#include <vector>
#include <algorithm>
#include <unordered_map>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "secpion/se_random.hpp"

using namespace secpion;

// Define test cases for multiple types:
// TEMPLATE_TEST_CASE(NAME, TAGS, TYPES...) { /* test code for TestType */ }
//
// Or use a templated function:
// test_random_numbers<G>() {}
//

template <typename G>
void gen_random_numbers() {
    G gen;

    // Test for 64 bit integer values
    for (uint16_t i = 0; i < 1000; i++) {
        uint64_t v = gen.get_uint64(5);
        REQUIRE(v < 5);
    }

    // Test for 32 bit integer values
    for (uint16_t i = 0; i < 1000; i++) {
        uint32_t v = gen.get_uint32(11);
        REQUIRE(v < 11);
    }

    // Test for 16 bit integer values
    for (uint16_t i = 0; i < 1000; i++) {
        uint16_t v = gen.get_uint16(17);
        REQUIRE(v < 17);
    }

    // Test for 8 bit integer values
    for (uint16_t i = 0; i < 1000; i++) {
        uint8_t v = gen.get_uint8(23);
        REQUIRE(v < 23);
    }

    // Test for size_t values
    for (uint16_t i = 0; i < 1000; i++) {
        size_t v = gen.get_size_t(29);
        REQUIRE(v < 29);
    }

    // Test for float64 values
    for (uint16_t i = 0; i < 1000; i++) {
        std::float64_t v = gen.get_float64();
        REQUIRE(v >= 0.0);
        REQUIRE(v <= 1.0);
    }

    // Check if all values are different:
    std::unordered_map<uint8_t, uint32_t> num_counter;
    for (uint16_t i = 0; i < 10000; i++) {
        uint8_t v = gen.get_uint8(100);
        num_counter[v]++;
    }

    REQUIRE(num_counter.size() > 50);

    for (auto& [key, value]: num_counter) {
        REQUIRE(value > 5);
    }
}

template <typename G>
void gen_seeds() {
    G gen1;
    G gen2;

    uint64_t v1 = gen1.get_uint64(10000000);
    uint64_t v2 = gen2.get_uint64(10000000);

    REQUIRE(v1 == v2);
    REQUIRE(gen1.get_uint8(255) == gen2.get_uint8(255));

    // Set a new seed for generator 1:
    gen1.seed();

    v1 = gen1.get_uint64(10000000);
    v2 = gen2.get_uint64(10000000);

    REQUIRE(v1 != v2);

    // Set a new seed for generator 2:
    gen2.seed();

    v1 = gen1.get_uint64(10000000);
    v2 = gen2.get_uint64(10000000);

    REQUIRE(v1 != v2);

    // Set same seed for both:
    gen1.seed(0);
    gen2.seed(0);

    v1 = gen1.get_uint64(10000000);
    v2 = gen2.get_uint64(10000000);

    REQUIRE(v1 == v2);
}

template <typename G>
void gen_shuffle_choice() {
    G gen;

    // Test shuffle:
    std::vector<uint8_t> v{};
    gen.shuffle(v);
    REQUIRE(v.size() == 0);

    v = {5};
    gen.shuffle(v);
    REQUIRE(v.size() == 1);
    REQUIRE(v == std::vector<uint8_t>{5});

    v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    gen.shuffle(v);
    REQUIRE(v.size() == 10);
    REQUIRE(v != std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    sort(v.begin(), v.end());
    REQUIRE(v == std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    // Test choice:
    v = {5};
    uint8_t n = gen.choice(v);
    REQUIRE(v == std::vector<uint8_t>{5});
    REQUIRE(n == 5);

    v = {5, 12};
    n = gen.choice(v);
    REQUIRE(v == std::vector<uint8_t>{5, 12});
    REQUIRE((n == 5) || (n == 12));

    v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    n = gen.choice(v);
    REQUIRE(v == std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    REQUIRE((n >= 1) && (n <= 10));

    // With empty vector:
    v = {};
    REQUIRE_THROWS_AS(gen.choice(v), SERNGException);
}

TEST_CASE("Create some random numbers", "[random]" ) {
    gen_random_numbers<SERandomGenerator<SEAlgorithmLehmer64>>();
    gen_random_numbers<SERandomGenerator<SEAlgorithmWyRand>>();
}

TEST_CASE("Test seed", "[random]" ) {
    gen_seeds<SERandomGenerator<SEAlgorithmLehmer64>>();
    gen_seeds<SERandomGenerator<SEAlgorithmWyRand>>();
}

TEST_CASE("Test shuffle and choice", "[random]" ) {
    gen_shuffle_choice<SERandomGenerator<SEAlgorithmLehmer64>>();
    gen_shuffle_choice<SERandomGenerator<SEAlgorithmLehmer64>>();
}

/*
C++ raw string literal:
std::string multi_line = R"(
        Line 1
        Line 2
        Line 3
    )";
*/
