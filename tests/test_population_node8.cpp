/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class type 8.

    Run only population type 8 tests:
    xmake run -w ./ se_test [population_type8]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>
#include <tao/json.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population_node8.hpp"

#include "utils.hpp"

using namespace secpion;

TEST_CASE("Test population type 8, run 1", "[population_type8]" ) {
    TestNP<SEPopulationNode8<TestRNG>> test_population;
    test_population.se_config.num_of_mutations = 1;
    SEPopulationNode8<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 8, run 2", "[population_type8]" ) {
    TestNP<SEPopulationNode8<TestRNG>> test_population;
    test_population.se_config.num_of_mutations = 1;
    SEPopulationNode8<TestRNG> population = test_population.run_inverted();
}

TEST_CASE("Test population type 8, run 3", "[population_type8]" ) {
    TestNP<SEPopulationNode8<TestRNG>> test_population;
    test_population.se_config.num_of_mutations = 5;
    SEPopulationNode8<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 8, run 4", "[population_type8]" ) {
    TestNP<SEPopulationNode8<TestRNG>> test_population;
    test_population.se_config.num_of_mutations = 1;
    test_population.se_config.accept_new_best = true;
    SEPopulationNode8<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 8, run 5", "[population_type8]" ) {
    TestNP<SEPopulationNode8<TestRNG>> test_population;
    test_population.se_config.num_of_mutations = 1;
    test_population.se_config.randomize_population = true;
    SEPopulationNode8<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 8, run 6", "[population_type8]" ) {
    TestNP<SEPopulationNode8<TestRNG>> test_population;
    test_population.se_config.num_of_mutations = 1;
    test_population.se_config.accept_new_best = true;
    test_population.se_config.randomize_population = true;
    SEPopulationNode8<TestRNG> population = test_population.run();
}
