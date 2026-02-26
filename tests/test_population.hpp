/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class.

    Run only configuration tests:
    xmake run -w ./ se_test [population]
*/

// STD includes:
#include <span>

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population.hpp"
#include "secpion/se_exceptions.hpp"

using namespace secpion;

std::unique_ptr<SEIndividual> make_indi_f1(std::float64_t fitness1) {
    std::unique_ptr<SEIndividual> individual = std::make_unique<SEIndividual>();
    individual->fitness1 = fitness1;
    return individual;
}

std::unique_ptr<SEIndividual> make_indi_f2(std::float64_t fitness2) {
    std::unique_ptr<SEIndividual> individual = std::make_unique<SEIndividual>();
    individual->fitness2 = fitness2;
    return individual;
}

std::unique_ptr<SEIndividual> make_indi_f1_f2(std::float64_t fitness1, std::float64_t fitness2) {
    std::unique_ptr<SEIndividual> individual = std::make_unique<SEIndividual>();
    individual->fitness1 = fitness1;
    individual->fitness2 = fitness2;
    return individual;
}

void check_population_fitness(SEPopulation &population,
    std::vector<std::float64_t> fitness1,
    std::vector<std::float64_t> fitness2) {
    size_t len1 = population.population.size();
    size_t len2 = fitness1.size();
    size_t len3 = fitness2.size();

    REQUIRE(len1 == len2);
    REQUIRE(len1 == len3);

    for (size_t i = 0; i < len1; i++) {
        REQUIRE(population.population[i]->fitness1 == fitness1[i]);
        REQUIRE(population.population[i]->fitness2 == fitness2[i]);
    }
}

TEST_CASE("Test find worst population", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    population.population.push_back(make_indi_f1_f2(23.5, 0.1));
    population.se_find_worst_individual();
    REQUIRE(population.worst_index == 0);
    check_population_fitness(population, {23.5}, {0.1});

    population.population.push_back(make_indi_f1_f2(58.9, 3.3));
    population.se_find_worst_individual();
    REQUIRE(population.worst_index == 1);
    check_population_fitness(population, {23.5, 58.9}, {0.1, 3.3});

    population.population[1]->fitness1 = 17.2;
    population.se_find_worst_individual();
    REQUIRE(population.worst_index == 0);
    check_population_fitness(population, {23.5, 17.2}, {0.1, 3.3});
}

TEST_CASE("Test find best and worst population", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    population.population.push_back(make_indi_f1_f2(23.5, 4.4));
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 0);
    REQUIRE(population.worst_index == 0);
    check_population_fitness(population, {23.5}, {4.4});

    population.population.push_back(make_indi_f1_f2(3.8, 5.5));
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 0);
    check_population_fitness(population, {23.5, 3.8}, {4.4, 5.5});

    population.population.push_back(make_indi_f1_f2(17.2, 6.6));
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 0);
    check_population_fitness(population, {23.5, 3.8, 17.2}, {4.4, 5.5, 6.6});

    population.population.push_back(make_indi_f1_f2(58.1, 7.7));
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 3);
    check_population_fitness(population, {23.5, 3.8, 17.2, 58.1}, {4.4, 5.5, 6.6, 7.7});

    population.population.push_back(make_indi_f1_f2(0.1, 8.8));
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 4);
    REQUIRE(population.worst_index == 3);
    check_population_fitness(population, {23.5, 3.8, 17.2, 58.1, 0.1}, {4.4, 5.5, 6.6, 7.7, 8.8});
}




/*
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