/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class type 6.

    Run only population type 6 tests:
    xmake run -w ./ se_test [population_type6]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>
#include <tao/json.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population_node6.hpp"

#include "utils.hpp"

using namespace secpion;

TEST_CASE("Test population type 6, run 1", "[population_type6]" ) {
    TestNP<SEPopulationNode6<TestRNG>> test_population;
    test_population.config2.num_of_mutations = 5;
    SEPopulationNode6<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 6, run 2", "[population_type6]" ) {
    TestNP<SEPopulationNode6<TestRNG>> test_population;
    test_population.config2.num_of_mutations = 5;
    SEPopulationNode6<TestRNG> population = test_population.run_inverted();
}

TEST_CASE("Test population type 6, run 3", "[population_type6]" ) {
    TestNP<SEPopulationNode6<TestRNG>> test_population;
    test_population.config2.num_of_mutations = 20;
    SEPopulationNode6<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 6, run 4", "[population_type6]" ) {
    TestNP<SEPopulationNode6<TestRNG>> test_population;
    test_population.config2.num_of_mutations = 5;
    test_population.config2.accept_new_best = true;
    SEPopulationNode6<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 6, run 5", "[population_type6]" ) {
    TestNP<SEPopulationNode6<TestRNG>> test_population;
    test_population.config2.num_of_mutations = 5;
    test_population.config2.randomize_population = true;
    SEPopulationNode6<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 6, run 6", "[population_type6]" ) {
    TestNP<SEPopulationNode6<TestRNG>> test_population;
    test_population.config2.num_of_mutations = 5;
    test_population.config2.accept_new_best = true;
    test_population.config2.randomize_population = true;
    SEPopulationNode6<TestRNG> population = test_population.run();
}
