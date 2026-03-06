/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains some helper classes and function for the test cases.
*/

#ifndef FILE_SE_TEST_UTILS_HPP_INCLUDED
#define FILE_SE_TEST_UTILS_HPP_INCLUDED

// External includes:
#include <tao/json.hpp>

// Local includes:
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"

SERandomGenerator<SEAlgorithmLehmer64> global_rng2;

using namespace secpion;

class TestIndividual3: public SEIndividual {
    public:
        std::vector<uint8_t> numbers;
        bool zero_is_optimal;

        TestIndividual3();
        void se_mutate(uint8_t) override;
        void se_randomize() override;
        void se_calculate_fitness1() override;
        std::unique_ptr<SEIndividual> se_clone() override;
        std::vector<uint8_t> se_to_vec_u8() override;
        void se_from_span_u8(std::span<const uint8_t>) override;
};

TestIndividual3::TestIndividual3():
    numbers(std::vector<uint8_t>(10)),
    zero_is_optimal(true) {
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
            if (numbers[i] < 10) {
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

    if (!zero_is_optimal) {
        fitness1 = 100.0 - fitness1;
    }
}

[[nodiscard]] std::unique_ptr<SEIndividual> TestIndividual3::se_clone() {
    std::unique_ptr<TestIndividual3> result = std::make_unique<TestIndividual3>();
    result->numbers = numbers;
    result->zero_is_optimal = zero_is_optimal;

    return result;
}

[[nodiscard]] std::vector<uint8_t> TestIndividual3::se_to_vec_u8() {
    tao::json::value json_numbers = tao::json::empty_array;

    for (uint8_t n: numbers) {
        json_numbers.get_array().push_back(n);
    }

    const tao::json::value json_data = {
        {"fitness1", double(fitness1)},
        {"fitness2", double(fitness2)},
        {"numbers", json_numbers},
        {"zero_is_optimal", zero_is_optimal}
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

    zero_is_optimal = restored_json["zero_is_optimal"].as<bool>();
}

#endif // FILE_SE_TEST_UTILS_HPP_INCLUDED
