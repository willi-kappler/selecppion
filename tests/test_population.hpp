/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class.

    Run only configuration tests:
    xmake run -w ./ se_test [population]
*/

// STD includes:
#include <cstring>

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population.hpp"
#include "secpion/se_exceptions.hpp"

using namespace secpion;

SERandomGenerator<SEAlgorithmLehmer64> global_rng;

const size_t FLOAT_64_SIZE = sizeof(std::float64_t);
const size_t TOTAL_DATA_SIZE = FLOAT_64_SIZE * 4;

class TestIndividual2: public SEIndividual {
    public:
        std::float64_t val1;
        std::float64_t val2;

        TestIndividual2();
        void se_mutate(uint8_t) override;
        void se_randomize() override;
        void se_calculate_fitness1() override;
        void se_calculate_fitness2() override;
        std::unique_ptr<SEIndividual> se_clone() override;
        std::vector<uint8_t> se_to_vec_u8() override;
        void se_from_span_u8(std::span<const uint8_t>) override;
        // std::float64_t se_actual_fitness() override;
        // void se_new_best_individual() override;
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

void TestIndividual2::se_calculate_fitness2() {
    fitness2 = val1 * val2;
}

std::unique_ptr<SEIndividual> TestIndividual2::se_clone() {
    std::unique_ptr<TestIndividual2> result = std::make_unique<TestIndividual2>();
    result->fitness1 = fitness1;
    result->fitness2 = fitness2;
    result->val1 = val1;
    result->val2 = val2;

    return result;
}

std::vector<uint8_t> TestIndividual2::se_to_vec_u8() {
    std::vector<uint8_t> result(TOTAL_DATA_SIZE);
    std::float64_t *float_64_ptr = reinterpret_cast<std::float64_t*>(result.data());

    float_64_ptr[0] = fitness1;
    float_64_ptr[1] = fitness2;
    float_64_ptr[2] = val1;
    float_64_ptr[3] = val2;

    return result;
}

void TestIndividual2::se_from_span_u8(std::span<const uint8_t> data) {
    if (data.size() < TOTAL_DATA_SIZE) {
        return;
    }

    const std::float64_t *float_64_ptr = reinterpret_cast<const std::float64_t*>(data.data());

    fitness1 = float_64_ptr[0];
    fitness2 = float_64_ptr[1];
    val1 = float_64_ptr[2];
    val2 = float_64_ptr[3];
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

void check_population_fitness1(SEPopulation &population,
    std::vector<std::float64_t> fitness1) {
    size_t len1 = population.population.size();
    size_t len2 = fitness1.size();

    REQUIRE(len1 == len2);

    for (size_t i = 0; i < len1; i++) {
        REQUIRE(population.population[i]->fitness1 == fitness1[i]);
    }
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

SEPopulation make_population(SEConfiguration config, uint8_t size) {
    SEPopulation population = SEPopulation(config);
    population.population.reserve(size);

    for (uint8_t i = 0; i < size; i++) {
        population.population.push_back(std::make_unique<TestIndividual2>());
    }

    return population;
}

SEPopulation make_population(uint8_t size) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);

    return make_population(config2, size);
}

void set_fitness1(SEPopulation &population, std::vector<std::float64_t> fitness1) {
    size_t len1 = population.population.size();
    size_t len2 = fitness1.size();

    REQUIRE(len1 == len2);

    for (size_t i = 0; i < population.population.size(); i++) {
        population.population[i]->fitness1 = fitness1[i];
    }
}

void set_fitness(SEPopulation &population, std::vector<std::float64_t> fitness1,
    std::vector<std::float64_t> fitness2) {

    size_t len1 = population.population.size();
    size_t len2 = fitness1.size();
    size_t len3 = fitness2.size();

    REQUIRE(len1 == len2);
    REQUIRE(len1 == len3);

    for (size_t i = 0; i < population.population.size(); i++) {
        population.population[i]->fitness1 = fitness1[i];
        population.population[i]->fitness2 = fitness2[i];
    }
}

void fill_fitness(SEPopulation &population, std::float64_t fitness1, std::float64_t fitness2) {
    for (auto &indi: population.population) {
        indi->fitness1 = fitness1;
        indi->fitness2 = fitness2;
    }
}

void set_values(SEPopulation &population, std::vector<std::float64_t> val1,
    std::vector<std::float64_t> val2) {
    size_t len1 = population.population.size();
    size_t len2 = val1.size();
    size_t len3 = val2.size();

    REQUIRE(len1 == len2);
    REQUIRE(len1 == len3);

    TestIndividual2 *test_indi;

    for (size_t i = 0; i < population.population.size(); i++) {
        test_indi = static_cast<TestIndividual2*>(population.population[i].get());

        test_indi->val1 = val1[i];
        test_indi->val2 = val2[i];
    }
}

void fill_values(SEPopulation &population, std::float64_t val1, std::float64_t val2) {
    TestIndividual2 *test_indi;

    for (auto &indi: population.population) {
        test_indi = static_cast<TestIndividual2*>(indi.get());
        test_indi->val1 = val1;
        test_indi->val2 = val2;
    }
}

TEST_CASE("Test fill population", "[population]" ) {
    SEPopulation population = make_population(0);
    population.se_config.node_population_size = 10;
    std::unique_ptr<TestIndividual2> individual = std::make_unique<TestIndividual2>();
    population.se_fill_population(std::move(individual));

    REQUIRE(population.se_config.node_population_size == 10);
    REQUIRE(population.population.size() == population.se_config.node_population_size);

    std::float64_t val1;
    std::float64_t val2;
    TestIndividual2 *test_indi;

    for (auto &indi: population.population) {
        test_indi = static_cast<TestIndividual2*>(indi.get());
        val1 = test_indi->val1;
        val2 = test_indi->val2;

        REQUIRE(val1 >= 0.0 && val1 <= 10.0);
        REQUIRE(val2 >= 0.0 && val2 <= 10.0);
        REQUIRE(indi->fitness1 == val1 + val2);
    }
}

TEST_CASE("Test find worst population", "[population]" ) {
    SEPopulation population = make_population(0);

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
    SEPopulation population = make_population(0);

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
    SEPopulation population = make_population(0);

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
    SEPopulation population = make_population(0);

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

TEST_CASE("Test randomize or accept best 1", "[population]" ) {
    SEPopulation population = make_population(10);
    population.se_config.randomize_population = false;
    population.se_config.accept_new_best = false;
    fill_fitness(population, 1.6, 9.2);

    population.se_randomize_or_accept_best({});

    // No change expected:
    TestIndividual2 *test_indi;

    for (auto &indi: population.population) {
        REQUIRE(indi->fitness1 == 1.6);
        REQUIRE(indi->fitness2 == 9.2);

        test_indi = static_cast<TestIndividual2*>(indi.get());
        REQUIRE(test_indi->val1 == -1.0);
        REQUIRE(test_indi->val2 == -1.0);
    }
}

TEST_CASE("Test randomize or accept best 2", "[population]" ) {
    SEPopulation population = make_population(10);
    population.se_config.randomize_population = true;
    population.se_config.accept_new_best = false;
    population.se_config.randomize_count = 1;
    fill_fitness(population, 3.3, 6.2);

    global_rng.seed();

    population.se_randomize_or_accept_best({});

    // No change expected:
    TestIndividual2 *test_indi;

    for (auto &indi: population.population) {
        REQUIRE(indi->fitness1 == 3.3);
        REQUIRE(indi->fitness2 == 6.2);

        test_indi = static_cast<TestIndividual2*>(indi.get());
        REQUIRE(test_indi->val1 == -1.0);
        REQUIRE(test_indi->val2 == -1.0);
    }

    population.se_randomize_or_accept_best({});

    // Change expected:
    std::float64_t val1;
    std::float64_t val2;

    for (auto &indi: population.population) {
        test_indi = static_cast<TestIndividual2*>(indi.get());
        val1 = test_indi->val1;
        val2 = test_indi->val2;

        REQUIRE(val1 >= 0.0 && val1 <= 10.0);
        REQUIRE(val2 >= 0.0 && val2 <= 10.0);

        REQUIRE(indi->fitness1 == val1 + val2);
        REQUIRE(indi->fitness2 == 6.2);
    }
}

TEST_CASE("Test randomize or accept best 3", "[population]" ) {
    SEPopulation population = make_population(10);
    population.se_config.randomize_population = false;
    population.se_config.accept_new_best = true;
    fill_fitness(population, 8.7, 4.5);
    population.population[0]->fitness1 = 1.2;
    population.population[0]->fitness2 = 5.7;

    TestIndividual2 best_individual;
    best_individual.fitness1 = 0.3;
    best_individual.fitness2 = 2.1;
    best_individual.val1 = 6.3;
    best_individual.val2 = 4.9;
    population.se_randomize_or_accept_best(best_individual.se_to_vec_u8());

    // Only individual at index 0 should be changed:
    TestIndividual2 *test_indi;

    for (size_t i = 0; i < population.population.size(); i++) {
        test_indi = static_cast<TestIndividual2*>(population.population[i].get());

        if (i == 0) {
            REQUIRE(test_indi->fitness1 == 0.3);
            REQUIRE(test_indi->fitness2 == 2.1);
            REQUIRE(test_indi->val1 == 6.3);
            REQUIRE(test_indi->val2 == 4.9);
        } else {
            REQUIRE(test_indi->fitness1 == 8.7);
            REQUIRE(test_indi->fitness2 == 4.5);
            REQUIRE(test_indi->val1 == -1.0);
            REQUIRE(test_indi->val2 == -1.0);
        }
    }
}

TEST_CASE("Test randomize or accept best 4", "[population]" ) {
    SEPopulation population = make_population(10);
    population.se_config.randomize_population = true;
    population.se_config.accept_new_best = true;
    population.se_config.randomize_count = 1;
    fill_fitness(population, 9.9, 8.8);

    global_rng.seed();

    TestIndividual2 best_individual;
    best_individual.fitness1 = 0.3;
    best_individual.fitness2 = 2.1;
    best_individual.val1 = 6.3;
    best_individual.val2 = 4.9;
    population.se_randomize_or_accept_best(best_individual.se_to_vec_u8());

    // No change expected:
    TestIndividual2 *test_indi;

    for (auto &indi: population.population) {
        REQUIRE(indi->fitness1 == 9.9);
        REQUIRE(indi->fitness2 == 8.8);

        test_indi = static_cast<TestIndividual2*>(indi.get());
        REQUIRE(test_indi->val1 == -1.0);
        REQUIRE(test_indi->val2 == -1.0);
    }

    best_individual.fitness1 = 0.3;
    best_individual.fitness2 = 2.1;
    best_individual.val1 = 6.3;
    best_individual.val2 = 4.9;
    population.se_randomize_or_accept_best(best_individual.se_to_vec_u8());

    // Change expected:
    std::float64_t val1;
    std::float64_t val2;

    for (auto &indi: population.population) {
        test_indi = static_cast<TestIndividual2*>(indi.get());
        val1 = test_indi->val1;
        val2 = test_indi->val2;

        REQUIRE(val1 >= 0.0 && val1 <= 10.0);
        REQUIRE(val2 >= 0.0 && val2 <= 10.0);

        REQUIRE(indi->fitness1 == val1 + val2);
        REQUIRE(indi->fitness2 == 8.8);
    }
}

TEST_CASE("Test shuffle mutation operations", "[population]" ) {
    SEPopulation population = make_population(0);
    population.se_config.mutation_operations = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    global_rng.seed();

    population.se_shuffle_mutation_operations();

    std::vector<uint8_t> initial_ops = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    REQUIRE(population.se_config.mutation_operations != initial_ops);
}

TEST_CASE("Test randomize worst", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_worst_individual();
    REQUIRE(population.worst_index == 3);

    global_rng.seed();

    population.se_randomize_worst();

    TestIndividual2 *test_indi;
    test_indi = static_cast<TestIndividual2*>(population.population[3].get());
    std::float64_t val1 = test_indi->val1;
    std::float64_t val2 = test_indi->val2;
    std::float64_t val_sum = val1 + val2;

    REQUIRE(val1 >= 0.0);
    REQUIRE(val2 >= 0.0);
    REQUIRE(test_indi->fitness1 == val_sum);
    REQUIRE(test_indi->fitness1 != 9.41);

    check_population_fitness1(population, {4.62, 1.74, 4.19, val_sum, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
}

TEST_CASE("Test replace best 1", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 3);

    population.se_replace_best(make_indi_f1_f2(1.5, 2.3));
    check_population_fitness1(population, {4.62, 1.5, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
}

TEST_CASE("Test replace best 2", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 3);

    population.se_replace_best(make_indi_f1_f2(1.8, 2.3));
    check_population_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
}

TEST_CASE("Test replace worst", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 3);

    population.se_replace_worst(make_indi_f1_f2(9.8, 4.3));
    check_population_fitness1(population, {4.62, 1.74, 4.19, 9.8, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});

    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 3);

    population.se_replace_worst(make_indi_f1_f2(2.2, 5.3));
    check_population_fitness1(population, {4.62, 1.74, 4.19, 2.2, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});

    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 8);

    population.se_replace_worst(make_indi_f1_f2(0.98, 1.3));
    check_population_fitness1(population, {4.62, 1.74, 4.19, 2.2, 7.42, 6.99, 6.02, 5.58, 0.98, 7.58});

    population.se_find_best_and_worst_individual();
    REQUIRE(population.best_index == 8);
    REQUIRE(population.worst_index == 9);
}

TEST_CASE("Test clone best to worst", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_best_and_worst_individual();

    population.se_clone_best_to_worst();
    check_population_fitness1(population, {4.62, 1.74, 4.19, 1.74, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
}

TEST_CASE("Test get best", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_best_and_worst_individual();

    const SEIndividual &result = population.se_get_best();

    REQUIRE(result.fitness1 == 1.74);
}

TEST_CASE("Test get best fitness", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_best_and_worst_individual();

    REQUIRE(population.se_get_best_fitness() == 1.74);
}

TEST_CASE("Test get worst fitness", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
    population.se_find_best_and_worst_individual();

    REQUIRE(population.se_get_worst_fitness() == 9.41);
}

TEST_CASE("Test get mutation operation", "[population]" ) {
    SEPopulation population = make_population(0);
    population.se_config.mutation_operations = {5, 8, 2, 11};
    population.mut_op_index = 0;

    REQUIRE(population.se_get_mut_op() == 8);
    REQUIRE(population.se_get_mut_op() == 2);
    REQUIRE(population.se_get_mut_op() == 11);
    REQUIRE(population.se_get_mut_op() == 5);
    REQUIRE(population.se_get_mut_op() == 8);
    REQUIRE(population.se_get_mut_op() == 2);
    REQUIRE(population.se_get_mut_op() == 11);
    REQUIRE(population.se_get_mut_op() == 5);
}

TEST_CASE("Test check limit", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness1(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});

    population.se_check_limit(make_indi_f1_f2(3.9, 3.3), 4.2, 2);
    check_population_fitness1(population, {4.62, 1.74, 3.9, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});

    population.se_check_limit(make_indi_f1_f2(3.6, 3.3), 1.7, 2);
    check_population_fitness1(population, {4.62, 1.74, 3.6, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});

    population.se_check_limit(make_indi_f1_f2(5.2, 3.3), 1.7, 2);
    check_population_fitness1(population, {4.62, 1.74, 3.6, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58});
}

TEST_CASE("Test calculate fitness2 1", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58},
                            {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0});

    population.se_find_best_and_worst_individual();
    population.se_calculate_fitness2();
    check_population_fitness(population, {4.62, 1.74, 4.19, 9.41, 7.42, 6.99, 6.02, 5.58, 7.94, 7.58},
                                         {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0});
    
    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 3);
}

TEST_CASE("Test calculate fitness2 2", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness(population, {4.62, 0.001, 4.19, 9.41, 7.42, 0.002, 6.02, 5.58, 7.94, 7.58},
                            {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0});

    set_values(population, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0},
                           {2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5, 11.5});

    population.se_find_best_and_worst_individual();
    population.se_calculate_fitness2();
    check_population_fitness(population, {4.62, 0.001, 4.19, 9.41, 7.42, 0.002, 6.02, 5.58, 7.94, 7.58},
                                         {1.1, 7.0, 1.3, 1.4, 1.5, 45.0, 1.7, 1.8, 1.9, 2.0});

    REQUIRE(population.best_index == 1);
    REQUIRE(population.worst_index == 3);
}

TEST_CASE("Test calculate fitness2 3", "[population]" ) {
    SEPopulation population = make_population(10);
    set_fitness(population, {4.62, 0.001, 4.19, 9.41, 7.42, 0.002, 6.02, 5.58, 7.94, 7.58},
                            {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0});

    set_values(population, {1.0, 6.0, 3.0, 4.0, 5.0, 2.0, 7.0, 8.0, 9.0, 10.0},
                           {2.5, 7.5, 4.5, 5.5, 6.5, 3.5, 8.5, 9.5, 10.5, 11.5});

    population.se_find_best_and_worst_individual();
    population.se_calculate_fitness2();
    check_population_fitness(population, {4.62, 0.001, 4.19, 9.41, 7.42, 0.002, 6.02, 5.58, 7.94, 7.58},
                                         {1.1, 45.0, 1.3, 1.4, 1.5, 7.0, 1.7, 1.8, 1.9, 2.0});

    REQUIRE(population.best_index == 5);
    REQUIRE(population.worst_index == 3);
}
