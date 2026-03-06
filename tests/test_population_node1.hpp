/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class type 1.

    Run only population type 1 tests:
    xmake run -w ./ se_test [population_type1]
*/

// STD includes:
#include <iostream>

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
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    config2.node_population_size = 10;
    config2.num_of_iterations = 1000;
    config2.mutation_operations = {0, 1, 2};
    config2.randomize_population = false;
    config2.accept_new_best = false;
    std::unique_ptr<TestIndividual3> individual1 = std::make_unique<TestIndividual3>();
    SEPopulationNode1 population(config2, std::move(individual1));

    TestIndividual3 individual2;
    individual2.numbers = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    individual2.se_calculate_fitness1();
    REQUIRE(individual2.fitness1 == 10.0);
    std::vector<uint8_t> result = population.nc_process_data(individual2.se_to_vec_u8());
    individual2.se_from_span_u8(result);

    REQUIRE(individual2.fitness1 == 0.0);
    std::vector<uint8_t> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    REQUIRE(individual2.numbers == expected);

    SEIndividual* individual3 = population.get_individual(config2.node_population_size - 1);
    REQUIRE(individual3->fitness1 > 0.0);

    /*
    for (size_t i = 0; i < config2.node_population_size; i++) {
        individual3 = population.get_individual(i);
        std::cout << "fitness1: " << individual3->fitness1 << std::endl;
    }
    */
}

TEST_CASE("Test population type 1, run 2", "[population_type1]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    config2.node_population_size = 10;
    config2.num_of_iterations = 1000;
    config2.mutation_operations = {0, 1, 2};
    config2.randomize_population = false;
    config2.accept_new_best = false;
    std::unique_ptr<TestIndividual3> individual1 = std::make_unique<TestIndividual3>();
    individual1->zero_is_optimal = false;
    SEPopulationNode1 population(config2, std::move(individual1));

    TestIndividual3 individual2;
    individual2.zero_is_optimal = false;
    individual2.numbers = {9, 9, 9, 9, 9, 9, 9, 9, 9, 9};
    individual2.se_calculate_fitness1();
    REQUIRE(individual2.fitness1 == 10.0);
    std::vector<uint8_t> result = population.nc_process_data(individual2.se_to_vec_u8());
    individual2.se_from_span_u8(result);

    REQUIRE(individual2.fitness1 == 0.0);
    std::vector<uint8_t> expected = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
    REQUIRE(individual2.numbers == expected);

    SEIndividual* individual3 = population.get_individual(config2.node_population_size - 1);
    REQUIRE(individual3->fitness1 > 0.0);
}
