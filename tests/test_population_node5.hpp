/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class type 5.

    Run only population type 5 tests:
    xmake run -w ./ se_test [population_type5]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>
#include <tao/json.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population_node5.hpp"

#include "utils.hpp"

using namespace secpion;

TEST_CASE("Test population type 5, run 1", "[population_type5]" ) {
    TestNP<SEPopulationNode5<TestRNG>> test_population;
    test_population.print_stats = true;
    SEPopulationNode5<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 5, run 2", "[population_type5]" ) {
    TestNP<SEPopulationNode5<TestRNG>> test_population;
    test_population.print_stats = true;
    SEPopulationNode5<TestRNG> population = test_population.run_inverted();
}

TEST_CASE("Test population type 5, run 3", "[population_type5]" ) {
    TestNP<SEPopulationNode5<TestRNG>> test_population;
    test_population.print_stats = true;
    test_population.config2.num_of_mutations = 5;
    SEPopulationNode5<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 5, run 4", "[population_type5]" ) {
    TestNP<SEPopulationNode5<TestRNG>> test_population;
    test_population.print_stats = true;
    test_population.config2.accept_new_best = true;
    SEPopulationNode5<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 5, run 5", "[population_type5]" ) {
    TestNP<SEPopulationNode5<TestRNG>> test_population;
    test_population.print_stats = true;
    test_population.config2.randomize_population = true;
    SEPopulationNode5<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 5, run 6", "[population_type5]" ) {
    TestNP<SEPopulationNode5<TestRNG>> test_population;
    test_population.print_stats = true;
    test_population.config2.accept_new_best = true;
    test_population.config2.randomize_population = true;
    SEPopulationNode5<TestRNG> population = test_population.run();
}
