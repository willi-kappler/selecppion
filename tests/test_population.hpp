/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class.

    Run only configuration tests:
    xmake run -w ./ se_test [population]
*/

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population.hpp"
#include "secpion/se_exceptions.hpp"

using namespace secpion;

TEST_CASE("Test find worst population", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    std::unique_ptr<SEIndividual> individual;
    individual->fitness1 = 23.5;
    population.population.push_back(std::move(individual));

    population.se_find_worst_individual();
    REQUIRE(population.worst_index == 0);

    individual = std::make_unique<SEIndividual>();
    individual->fitness1 = 58.9;
    population.population.push_back(std::move(individual));

    population.se_find_worst_individual();
    REQUIRE(population.worst_index == 1);

    population.population[1]->fitness1 = 17.2;

    population.se_find_worst_individual();
    REQUIRE(population.worst_index == 0);
}

/*

void se_find_worst_individual();
void se_find_best_and_worst_individual();
void se_sort_population();
void se_random_population();
void se_randomize_or_accept_best(std::unique_ptr<SEIndividual>);
void se_shuffle_mutation_operations();
void se_randomize_worst();
void se_replace_best(std::unique_ptr<SEIndividual>);
void se_replace_worst(std::unique_ptr<SEIndividual>);
void se_clone_best_to_worst();
const SEIndividual& se_get_best() const;
std::float64_t se_get_best_fitness();
std::float64_t se_get_worst_fitness();
uint8_t se_get_mut_op();
void se_check_limit(std::unique_ptr<SEIndividual>, std::float64_t, size_t);
void se_early_exit(uint64_t);
void se_calculate_fitness2();
void se_log_statistics();

*/