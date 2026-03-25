/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the queens example

    To just build use:
    xmake build se_example_queens

    Run with:
    ./run_example.sh
*/

// STD includes:
#include <vector>
#include <tuple>
#include <stdfloat>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;
const uint8_t BOARD_SIZE = 8;

class QueensIndividual: public SEIndividual {
    public:
        std::vector<uint8_t> columns;

        QueensIndividual():
        columns(std::vector<uint8_t>(BOARD_SIZE, 0))
        {}

        void swap_items() {
            auto [i1, i2] = global_rng.get_two_size_t(BOARD_SIZE);
            std::swap(columns[i1], columns[i2]);
        }

        void set_row() {
            size_t i = global_rng.get_size_t(BOARD_SIZE);
            columns[i] = global_rng.get_uint8(BOARD_SIZE);
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    swap_items();
                    break;
                default:
                    set_row();
                    break;
            }
        }

        void se_randomize() override {
            for (size_t i = 0; i < BOARD_SIZE; i++) {
                columns[i] = global_rng.get_uint8(BOARD_SIZE);
            }
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;
            std::set<uint8_t> rows{};

            for (size_t i = 0; i < BOARD_SIZE; i++) {
                if (rows.contains(columns[i])) {
                    fitness1 += 1.0;
                } else {
                    rows.insert(columns[i]);
                }
            }

            // TODO: check diagonals.
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<QueensIndividual> result = std::make_unique<QueensIndividual>();

            result->columns = columns;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_arrays = tao::json::empty_array;

            // TODO
            const tao::json::value json_data = {
                {"fitness1", double(fitness1)}
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();
            // TODO
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            const QueensIndividual* const other_individual = dynamic_cast<const QueensIndividual* const>(individual);
            std::unique_ptr<QueensIndividual> result = std::make_unique<QueensIndividual>();

            auto [i1, i2] = global_rng.get_two_size_t(BOARD_SIZE);

            for (size_t i = 0; i < BOARD_SIZE; i++) {
                if ((i >= i1) && (i < i2)) {
                    result->columns[i] = columns[i];
                } else {
                    result->columns[i] = other_individual->columns[i];
                }
            }

            return result;
        }
};
