/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the sudoku2 example

    To just build use:
    xmake build se_example_sudoku2

    Run with:
    ./run_example.sh
*/

// STD includes:
#include <unordered_set>
#include <stdexcept>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"
#include "secpion/se_utils.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

class SudokuIndividual: public SEIndividual {
    public:
        std::vector<uint8_t> fixed_numbers;
        std::vector<uint8_t> solution;

        SudokuIndividual(std::vector<uint8_t> input):
        fixed_numbers(input),
        solution(std::vector<uint8_t>(81, 0))
        {
            if (fixed_numbers.size() != 81) {
                throw std::runtime_error("Input size must be 81 numbers!");
            }
        }

        uint8_t get_fixed_number(size_t x, size_t y) {
            return fixed_numbers[(y * 9) + x];
        }

        uint8_t get_number(size_t x, size_t y) {
            return solution[(y * 9) + x];
        }

        void set_number(size_t x, size_t y, uint8_t number) {
            solution[(y * 9) + x] = number;
        }

        void allowed_numbers_in_row(size_t row, std::unordered_set<uint8_t> &numbers) {
            for (size_t x = 0; x < 9; x++) {
                numbers.erase(get_number(x, row));
            }
        }

        void allowed_numbers_in_col(size_t col, std::unordered_set<uint8_t> &numbers) {
            for (size_t y = 0; y < 9; y++) {
                numbers.erase(get_number(col, y));
            }
        }

        void allowed_numbers_in_block(size_t x, size_t y, std::unordered_set<uint8_t> &numbers) {
            x = (x / 3) * 3;
            y = (y / 3) * 3;

            for (size_t i = 0; i < 3; i++) {
                for (size_t j = 0; j < 3; j++) {
                    numbers.erase(get_number(x + i, y + j));
                }
            }
        }

        void se_mutate([[maybe_unused]] uint8_t op) override {
            size_t x, y, num_of_elems;
            std::unordered_set<uint8_t> allowed_numbers;
            std::vector<uint8_t> random_numbers;

            while (true) {
                x = global_rng.get_size_t(9);
                y = global_rng.get_size_t(9);

                if (get_fixed_number(x, y) == 0) {
                    allowed_numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};

                    allowed_numbers_in_row(y, allowed_numbers);
                    allowed_numbers_in_col(x, allowed_numbers);
                    allowed_numbers_in_block(x, y, allowed_numbers);

                    num_of_elems = allowed_numbers.size();

                    switch(num_of_elems) {
                        case 0:
                            // Got stuck, no solution possible.
                            // Just reset!
                            se_randomize();
                        break;
                        case 1:
                            random_numbers.assign(allowed_numbers.begin(), allowed_numbers.end());
                            set_number(x, y, random_numbers[0]);
                        break;
                        default:
                            random_numbers.assign(allowed_numbers.begin(), allowed_numbers.end());
                            global_rng.shuffle(random_numbers);
                            set_number(x, y, random_numbers[0]);
                        break;
                    }

                    break;
                }
            }
        }

        void se_randomize() override {
            for (size_t i = 0; i < 81; i++) {
                // Does not randomize, but reset the sudoku:
                solution[i] = fixed_numbers[i];
            }
        }

        std::float64_t check_block(size_t x, size_t y) {
            std::float64_t result = 0.0;
            size_t i, j;
            uint8_t number;
            std::unordered_set<uint8_t> numbers;

            for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                    number = get_number(x + i, y + j);

                    if (numbers.contains(number)) {
                        result += 1.0;
                    } else {
                        numbers.insert(number);
                    }
                }
            }

            return result;
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;
            size_t i, j;
            uint8_t number;
            std::unordered_set<uint8_t> numbers;

            // Check rows:
            for (j = 0; j < 9; j++) {
                numbers.clear();

                // Go through each column:
                for (i = 0; i < 9; i++) {
                    number = get_number(i, j);
                    if (numbers.contains(number)) {
                        fitness1 += 1.0;
                    } else {
                        numbers.insert(number);
                    }
                }
            }

            // Check columns:
            for (i = 0; i < 9; i++) {
                numbers.clear();

                // Go through each row:
                for (j = 0; j < 9; j++) {
                    number = get_number(i, j);
                    if (numbers.contains(number)) {
                        fitness1 += 1.0;
                    } else {
                        numbers.insert(number);
                    }
                }
            }

            // Check blocks:
            for (i = 0; i < 9; i+=3) {
                for (j = 0; j < 9; j+=3) {
                    fitness1 += check_block(i, j);
                }
            }
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<SudokuIndividual> result = std::make_unique<SudokuIndividual>(fixed_numbers);

            result->solution = solution;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"solution", se_vec_to_json<uint8_t>(solution)}
            };

            return se_json_to_vec_u8(json_data);
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            tao::json::value restored_json = se_span_u8_to_json(data);
            fitness1 = restored_json["fitness1"].as<double>();
            se_json_to_vec(restored_json["solution"], solution);
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
