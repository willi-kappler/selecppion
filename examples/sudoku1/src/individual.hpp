/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the sudoku example

    To just build use:
    xmake build se_example_sudoke

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

class SudokuIndividual: public SEIndividual {
    public:
        std::vector<uint8_t> fixed_numbers;
        std::vector<uint8_t> solution;

        SudokuIndividual(std::vector<uint8_t> input):
        fixed_numbers(input),
        solution(std::vector<uint8_t>(81, 0))
        {}

        uint8_t get_fixed_number(size_t x, size_t y) {
            return fixed_numbers[(y * 9) + x];
        }

        uint8_t get_number(size_t x, size_t y) {
            return solution[(y * 9) + x];
        }

        void set_number(size_t x, size_t y, uint8_t number) {
            if (get_fixed_number(x, y) == 0) {
                solution[(y * 9) + x] = number;
            }
        }

        void random_number() {
            while (true) {
                size_t i = global_rng.get_size_t(81);

                if (fixed_numbers[i] == 0) {
                    solution[i] = global_rng.get_uint8(9) + 1;
                    break;
                }
            }
        }

        void swap_numbers() {
            while (true) {
                auto [i1, i2] = global_rng.get_two_size_t(81);

                if ((fixed_numbers[i1] == 0) && (fixed_numbers[i2] == 0)) {
                    std::swap(solution[i1], solution[i2]);
                    break;
                }
            }
        }

        void se_mutate(uint8_t op) override {
            switch(op) {
                case 0:
                    random_number();
                    break;
                case 1:
                    swap_numbers();
                    break;
                default:
                    break;
            }
        }

        void se_randomize() override {
            for (size_t i = 0; i < 81; i++) {
                if (fixed_numbers[i] == 0) {
                    solution[i] = global_rng.get_uint8(9) + 1;
                }
            }
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<SudokuIndividual> result = std::make_unique<SudokuIndividual>(fixed_numbers);

            result->solution = solution;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_array = tao::json::empty_array;

            for (uint8_t s: solution) {
                json_array.get_array().push_back(s);
            }

            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"solution", json_array}
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();

            const auto& arr1 = restored_json["solution"].get_array();

            for (size_t i = 0; i < 81; i++) {
                solution[i] = arr1[i].as<uint8_t>();
            }
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            const SudokuIndividual* const other_individual = dynamic_cast<const SudokuIndividual* const>(individual);
            std::unique_ptr<SudokuIndividual> result = std::make_unique<SudokuIndividual>(fixed_numbers);

            auto [i1, i2] = global_rng.get_two_size_t(81);

            for (size_t i = 0; i < 81; i++) {
                if ((i >= i1) && (i < i2)) {
                    result->solution[i] = solution[i];
                } else {
                    result->solution[i] = other_individual->solution[i];
                }
            }

            return result;
        }
};
