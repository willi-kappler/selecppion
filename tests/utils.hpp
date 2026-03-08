/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains some helper classes and function for the test cases.
*/

#ifndef FILE_SE_TEST_UTILS_HPP_INCLUDED
#define FILE_SE_TEST_UTILS_HPP_INCLUDED

// STD include:
#include <print>

// External includes:
#include <tao/json.hpp>
#include <spdlog/spdlog.h>

// Local includes:
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"

typedef SERandomGenerator<SEAlgorithmLehmer64> TestRNG;
//typedef SERandomGenerator<SEAlgorithmWyRand> TestRNG;
//typedef SERandomGenerator<SEAlgorithmSTD_LCG> TestRNG;
//typedef SERandomGenerator<SEAlgorithmSTD_Mersenne> TestRNG;
//typedef SERandomGenerator<SEAlgorithmSTD_SWC> TestRNG;

TestRNG global_rng;

using namespace secpion;

class TestIndividual1: public SEIndividual {
    public:
        uint32_t mutate_called;
        uint32_t clone_called;

        std::vector<uint8_t> numbers;
        bool zero_is_optimal;

        TestIndividual1():
            mutate_called(0),
            clone_called(0),
            numbers(std::vector<uint8_t>(10)),
            zero_is_optimal(true) {
            se_randomize();
        }

        void se_mutate(uint8_t op) override {
            mutate_called++;

            size_t i = global_rng.get_size_t(numbers.size());
            uint8_t val;

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
                case 2:
                    val = global_rng.get_uint8(10);
                    numbers[i] = val;

                    break;
                default:
                    val = global_rng.get_uint8(10);

                    for (uint8_t &number: numbers) {
                        number = val;
                    }

                    break;
            }
        }

        void se_randomize() override {
            for (size_t i = 0; i < numbers.size(); i++) {
                numbers[i] = global_rng.get_uint8(10);
            }
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;

            for (uint8_t n: numbers) {
                fitness1 += n;
            }

            if (!zero_is_optimal) {
                fitness1 = 100.0 - fitness1;
            }
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            clone_called++;

            std::unique_ptr<TestIndividual1> result = std::make_unique<TestIndividual1>();
            //result->mutate_called = mutate_called;
            //result->clone_called = clone_called;
            result->numbers = numbers;
            result->zero_is_optimal = zero_is_optimal;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_numbers = tao::json::empty_array;

            for (uint8_t n: numbers) {
                json_numbers.get_array().push_back(n);
            }

            const tao::json::value json_data = {
                {"mutate_called", mutate_called},
                {"clone_called", clone_called},
                {"fitness1", double(fitness1)},
                {"fitness2", double(fitness2)},
                {"numbers", json_numbers},
                {"zero_is_optimal", zero_is_optimal}
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            mutate_called = restored_json["mutate_called"].as<uint32_t>();
            clone_called = restored_json["clone_called"].as<uint32_t>();
            fitness1 = restored_json["fitness1"].as<double>();
            fitness2 = restored_json["fitness2"].as<double>();

            const auto& arr = restored_json["numbers"].get_array();

            for (size_t i = 0; i < numbers.size(); i++) {
                numbers[i] = arr[i].as<uint8_t>();
            }

            zero_is_optimal = restored_json["zero_is_optimal"].as<bool>();
        }
};

template<typename T>
class TestNP {
    public:
        NCConfiguration config1;
        SEConfiguration config2;
        std::unique_ptr<TestIndividual1> individual1;
        TestIndividual1 individual2;
        std::vector<uint8_t> expected;
        bool print_stats;

        TestNP():
            config1(NCConfiguration("12345678901234567890123456789012")),
            config2(SEConfiguration(config1)),
            individual1(std::make_unique<TestIndividual1>()),
            individual2(),
            expected({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
            print_stats(true)
        {
            config2.node_population_size = 10;
            config2.num_of_iterations = 100;
            config2.mutation_operations = {0, 1, 2, 3};
            config2.num_of_mutations = 1;
            config2.randomize_population = false;
            config2.accept_new_best = false;
            config2.early_exit_sleep = 0;
            individual2.numbers = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
            individual2.se_calculate_fitness1();
        }

        [[nodiscard]] T run() {
            T population(config2, individual1->se_clone());
            population.se_set_loglevel(spdlog::level::level_enum::debug);
            std::vector<uint8_t> result = population.nc_process_data(individual2.se_to_vec_u8());
            individual2.se_from_span_u8(result);
            REQUIRE(individual2.fitness1 == 0.0);
            REQUIRE(individual2.numbers == expected);
            SEIndividual* individual3 = population.se_get_worst();
            REQUIRE(individual3->fitness1 > 0.0);

            if (print_stats) {
                TestIndividual1* individual4;

                for (size_t i = 0; i < config2.node_population_size; i++) {
                    individual4 = static_cast<TestIndividual1*>(population.se_get_individual(i));
                    std::print("fitness: {}, mutate_called: {}, clone_called: {}, mut ops: {}\n",
                        individual4->fitness1, individual4->mutate_called,
                        individual4->clone_called, individual4->mut_op_counter);
                }
            }

            std::print("\n");

            return population;
        }

        [[nodiscard]] T run_inverted() {
            individual1->zero_is_optimal = false;
            individual2.zero_is_optimal = false;
            individual2.numbers = {9, 9, 9, 9, 9, 9, 9, 9, 9, 9};
            individual2.se_calculate_fitness1();
            expected = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

            return run();
        }
};

#endif // FILE_SE_TEST_UTILS_HPP_INCLUDED
