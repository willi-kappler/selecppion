/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class type 11.

    Run only population type 11 tests:
    xmake run -w ./ se_test [population_type11]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>
#include <tao/json.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population_node11.hpp"

#include "utils.hpp"

using namespace secpion;

TEST_CASE("Test population type 11, run 1", "[population_type11]" ) {
    TestNP<SEPopulationNode11<TestRNG>> test_population;
    // test_population.print_stats = true;
    test_population.se_config.num_of_mutations = 1;
    SEPopulationNode11<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 11, run 2", "[population_type11]" ) {
    TestNP<SEPopulationNode11<TestRNG>> test_population;
    // test_population.print_stats = true;
    test_population.se_config.num_of_mutations = 1;
    SEPopulationNode11<TestRNG> population = test_population.run_inverted();
}

TEST_CASE("Test population type 11, run 3", "[population_type11]" ) {
    TestNP<SEPopulationNode11<TestRNG>> test_population;
    // test_population.print_stats = true;
    test_population.se_config.num_of_mutations = 5;
    SEPopulationNode11<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 11, run 4", "[population_type11]" ) {
    TestNP<SEPopulationNode11<TestRNG>> test_population;
    // test_population.print_stats = true;
    test_population.se_config.num_of_mutations = 1;
    test_population.se_config.accept_new_best = true;
    SEPopulationNode11<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 11, run 5", "[population_type11]" ) {
    TestNP<SEPopulationNode11<TestRNG>> test_population;
    // test_population.print_stats = true;
    test_population.se_config.num_of_mutations = 1;
    test_population.se_config.randomize_population = true;
    SEPopulationNode11<TestRNG> population = test_population.run();
}

TEST_CASE("Test population type 11, run 6", "[population_type11]" ) {
    TestNP<SEPopulationNode11<TestRNG>> test_population;
    // test_population.print_stats = true;
    test_population.se_config.num_of_mutations = 1;
    test_population.se_config.accept_new_best = true;
    test_population.se_config.randomize_population = true;
    SEPopulationNode11<TestRNG> population = test_population.run();
}
