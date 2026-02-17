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

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "secpion/se_random.hpp"
// #include "secpion/se_exceptions.hpp"

using namespace secpion;

TEST_CASE("Create some random numbers using Lehmer64", "[random]" ) {
    SERandomGenerator<SEAlgorithmLehmer64> gen = SERandomGenerator<SEAlgorithmLehmer64>();

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
}

TEST_CASE("Test seed using Lehmer64", "[random]" ) {
    SERandomGenerator<SEAlgorithmLehmer64> gen1 = SERandomGenerator<SEAlgorithmLehmer64>();
    SERandomGenerator<SEAlgorithmLehmer64> gen2 = SERandomGenerator<SEAlgorithmLehmer64>();

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

TEST_CASE("Test shuffle and choice using Lehmer64", "[random]" ) {
    SERandomGenerator<SEAlgorithmLehmer64> gen = SERandomGenerator<SEAlgorithmLehmer64>();

    std::vector<uint8_t> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    gen.shuffle(v);

    REQUIRE(v != std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

}

/*
C++ raw string literal:
std::string multi_line = R"(
        Line 1
        Line 2
        Line 3
    )";
*/

/*
Test for exception:

TEST_CASE("Create invalid default configuration", "[configuration]" ) {
    REQUIRE_THROWS_AS(NCConfiguration("12345"), NCInvalidKeyException);
}


*/