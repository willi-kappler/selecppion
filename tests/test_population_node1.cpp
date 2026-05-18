/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class type 1.

    Run only population type 1 tests:
    xmake run -w ./ se_test [population_type1]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>
#include <tao/json.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population_node1.hpp"

#include "utils.hpp"

using namespace secpion;

TEST_CASE("Test population type 1, run 1", "[population_type1]" ) {
    TestNP<SEPopulationNode1<TestRNG>> test_population;
    SEPopulationNode1<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 1, run 2", "[population_type1]" ) {
    TestNP<SEPopulationNode1<TestRNG>> test_population;
    SEPopulationNode1<TestRNG> population = test_population.run_inverted();
}

TEST_CASE("Test population type 1, run 3", "[population_type1]" ) {
    TestNP<SEPopulationNode1<TestRNG>> test_population;
    test_population.se_config.num_of_mutations = 5;
    SEPopulationNode1<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 1, run 4", "[population_type1]" ) {
    TestNP<SEPopulationNode1<TestRNG>> test_population;
    test_population.se_config.accept_new_best = true;
    SEPopulationNode1<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 1, run 5", "[population_type1]" ) {
    TestNP<SEPopulationNode1<TestRNG>> test_population;
    test_population.se_config.randomize_population = true;
    SEPopulationNode1<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 1, run 6", "[population_type1]" ) {
    TestNP<SEPopulationNode1<TestRNG>> test_population;
    test_population.se_config.accept_new_best = true;
    test_population.se_config.randomize_population = true;
    SEPopulationNode1<TestRNG> population = test_population.run();
}
