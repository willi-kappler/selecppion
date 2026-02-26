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

SERandomGenerator<SEAlgorithmLehmer64> global_rng;

class TestIndividual2: public SEIndividual {
    public:
        std::float64_t val1;
        std::float64_t val2;

        TestIndividual2();
        void se_mutate(uint8_t) override;
        void se_randomize() override;
        void se_calculate_fitness1() override;


/*
virtual void se_calculate_fitness2();
virtual std::unique_ptr<SEIndividual> se_clone();
virtual void se_from_server(std::unique_ptr<SEIndividual>);
virtual tao::json::value se_to_json();
virtual void se_from_json(const tao::json::value);
virtual std::float64_t se_actual_fitness();
virtual void se_new_best_individual();
*/
};

TestIndividual2::TestIndividual2():
    val1(-1.0),
    val2(-1.0)
    {}

void TestIndividual2::se_mutate([[maybe_unused]] uint8_t mut_op) {
    std::float64_t delta = global_rng.get_float64();

    if (delta <= 0.5) {
        val1 += ((delta - 0.5) * 0.01);

        if (val1 < 0.0) {
            val1 = 0.0;
        } else if (val1 > 10.0) {
            val1 = 10.0;
        }
    } else {
        val2 += ((delta - 0.5) * 0.01);

        if (val2 < 0.0) {
            val2 = 0.0;
        } else if (val2 > 10.0) {
            val2 = 10.0;
        }
    }
}

void TestIndividual2::se_randomize() {
    val1 = global_rng.get_float64() * 10.0;
    val2 = global_rng.get_float64() * 10.0;
}

void TestIndividual2::se_calculate_fitness1() {
    fitness1 = val1 + val2;
}

std::unique_ptr<SEIndividual> make_indi_f1_f2(std::float64_t fitness1, std::float64_t fitness2) {
    std::unique_ptr<SEIndividual> individual = std::make_unique<SEIndividual>();
    individual->fitness1 = fitness1;
    individual->fitness2 = fitness2;
    return individual;
}

std::unique_ptr<TestIndividual2> make_test_indi_f1_f2(std::float64_t fitness1, std::float64_t fitness2) {
    std::unique_ptr<TestIndividual2> individual = std::make_unique<TestIndividual2>();
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

TEST_CASE("Test sort population", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    population.population.push_back(make_indi_f1_f2(23.5, 4.4));
    population.population.push_back(make_indi_f1_f2(3.8, 5.5));
    population.population.push_back(make_indi_f1_f2(17.2, 6.6));
    population.population.push_back(make_indi_f1_f2(58.1, 7.7));
    population.population.push_back(make_indi_f1_f2(0.1, 8.8));

    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 4);
    REQUIRE(population.worst_index == 3);

    population.se_sort_population();
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 0);
    REQUIRE(population.worst_index == 4);
    check_population_fitness(population, {0.1, 3.8, 17.2, 23.5, 58.1}, {8.8, 5.5, 6.6, 4.4, 7.7});
}

TEST_CASE("Test random population", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();

    population.population.push_back(make_test_indi_f1_f2(1023.5, 4.4));
    population.population.push_back(make_test_indi_f1_f2(103.8, 5.5));
    population.population.push_back(make_test_indi_f1_f2(1017.2, 6.6));
    population.population.push_back(make_test_indi_f1_f2(1058.1, 7.7));
    population.population.push_back(make_test_indi_f1_f2(100.1, 8.8));

    for (uint8_t i = 0; i < population.population.size(); i++) {
        population.population[i]->mut_op_counter[i] = i;
    }

    population.se_random_population();

    std::float64_t val1;
    std::float64_t val2;

    TestIndividual2 *test_indi;

    for (auto &individual: population.population) {
        REQUIRE(individual->mut_op_counter.size() == 0);
        REQUIRE(individual->fitness1 >= 0.0);
        REQUIRE(individual->fitness1 <= 20.0);

        test_indi = static_cast<TestIndividual2*>(individual.get());
        val1 = test_indi->val1;
        val2 = test_indi->val2;

        REQUIRE(val1 >= 0 && val1 <= 10.0);
        REQUIRE(val2 >= 0 && val2 <= 10.0);
        REQUIRE(test_indi->fitness1 = val1 + val2);
    }
}

TEST_CASE("Test randomize or accept best", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test shuffle mutation operations", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test randomize worst", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test replace best", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test replace worst", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test clone best to worst", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test get best", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test get best fitness", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

TEST_CASE("Test get worst fitness", "[population]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);
    SEPopulation population = SEPopulation(config2);

    global_rng.seed();
}

/*
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