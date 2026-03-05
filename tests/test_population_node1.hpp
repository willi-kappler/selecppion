/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the population class.

    Run only configuration tests:
    xmake run -w ./ se_test [population_type1]
*/

// STD includes:
#include <iostream>

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>
#include <tao/json.hpp>
// #include <tao/json/contrib/vector_traits.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_population_node1.hpp"

SERandomGenerator<SEAlgorithmLehmer64> global_rng2;

using namespace secpion;

class TestIndividual3: public SEIndividual {
    public:
        std::vector<uint8_t> numbers;

        TestIndividual3();
        void se_mutate(uint8_t) override;
        void se_randomize() override;
        void se_calculate_fitness1() override;
        std::unique_ptr<SEIndividual> se_clone() override;
        std::vector<uint8_t> se_to_vec_u8() override;
        void se_from_span_u8(std::span<const uint8_t>) override;
};

TestIndividual3::TestIndividual3():
    numbers(std::vector<uint8_t>(10)) {
    se_randomize();
}

void TestIndividual3::se_mutate(uint8_t op) {
    size_t i = global_rng2.get_size_t(numbers.size());

    switch (op) {
        case 0:
            if (numbers[i] > 0) {
                numbers[i]--;
            }
            break;
        case 1:
            if (numbers[i] < 9) {
                numbers[i]++;
            }
            break;
        default:
            uint8_t val = global_rng2.get_uint8(10);
            numbers[i] = val;
            break;
    }

}

void TestIndividual3::se_randomize() {
    for (size_t i = 0; i < numbers.size(); i++) {
        numbers[i] = global_rng2.get_uint8(10);
    }
}

void TestIndividual3::se_calculate_fitness1() {
    fitness1 = 0.0;

    for (uint8_t n: numbers) {
        fitness1 += n;
    }
}

std::unique_ptr<SEIndividual> TestIndividual3::se_clone() {
    std::unique_ptr<TestIndividual3> result = std::make_unique<TestIndividual3>();
    result->numbers = numbers;

    return result;
}

std::vector<uint8_t> TestIndividual3::se_to_vec_u8() {
    tao::json::value json_numbers = tao::json::empty_array;
    
    for (uint8_t n: numbers) {
        json_numbers.get_array().push_back(n);
    }

    const tao::json::value json_data = {
        {"fitness1", double(fitness1)},
        {"fitness2", double(fitness2)},
        {"numbers", json_numbers}
    };

    std::string serialized = tao::json::to_string(json_data);
    std::vector<uint8_t> result(serialized.begin(), serialized.end());

    return result;
}

void TestIndividual3::se_from_span_u8(std::span<const uint8_t> data) {
    const char* data_ptr = reinterpret_cast<const char*>(data.data());
    tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

    fitness1 = restored_json["fitness1"].as<double>();
    fitness2 = restored_json["fitness2"].as<double>();

    const auto& arr = restored_json["numbers"].get_array();

    for (size_t i = 0; i < numbers.size(); i++) {
        numbers[i] = arr[i].as<uint8_t>();
    }
}

TEST_CASE("Test population type 1", "[population_type1]" ) {
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

    for (size_t i = 0; i < config2.node_population_size; i++) {
        individual3 = population.get_individual(i);
        std::cout << "fitness1: " << individual3->fitness1 << std::endl;
    }
}
