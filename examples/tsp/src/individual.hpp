/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the TSP example

    To just build use:
    xmake build se_example_tsp

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

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;
std::vector<std::tuple<std::float64_t, std::float64_t>> global_positions;

void load_data(std::string_view filename) {
    std::string city_positions = se_file_to_string(filename);
    std::istringstream sstream(city_positions);
    std::float64_t x, y;

    global_positions.clear();

    while (sstream >> x >> y) {
        global_positions.push_back({x, y});
    }
}

class TSPIndividual: public SEIndividual {
    public:
        std::vector<size_t> position_indices;

        TSPIndividual():
        position_indices()
        {}

        void init_positions() {
            const size_t end = global_positions.size();
            position_indices.reserve(end);

            for (size_t i = 0; i < end; i++) {
                position_indices.push_back(i);
            }
        }

        size_t get_one_index() {
            return global_rng.get_size_t(position_indices.size());
        }

        std::tuple<size_t, size_t> get_two_indices() {
            size_t size = position_indices.size();
            size_t i1 = global_rng.get_size_t(size);
            size_t i2 = global_rng.get_size_t(size);

            while (i1 == i2) {
                i2 = global_rng.get_size_t(size);
            }

            return {i1, i2};
        }

        void reverse() {
            auto [i1, i2] = get_two_indices();

            if (i1 > i2) {
                std::swap(i1, i2);
            }

            std::reverse(position_indices.begin() + i1, position_indices.begin() + i2);
        }

        void just_swap() {
            global_rng.swap(position_indices);
        }

        void shift_left() {
            auto [i1, i2] = get_two_indices();

            if (i1 > i2) {
                std::swap(i1, i2);
            }

            std::rotate(position_indices.begin() + i1,
                position_indices.begin() + i1 + 1, position_indices.begin() + i2);
        }

        void shift_right() {
            auto [i1, i2] = get_two_indices();

            if (i1 > i2) {
                std::swap(i1, i2);
            }

            std::rotate(position_indices.begin() + i1,
                position_indices.begin() + i2 - 1, position_indices.begin() + i2);
        }

        void try_best1() {
            size_t i1 = get_one_index();
            std::float64_t current_best_fitness1 = fitness1;
            auto current_best_posisions = position_indices;

            for (size_t j = 0; j < position_indices.size(); j++) {
                if (i1 != j) {
                    std::swap(position_indices[i1], position_indices[j]);
                    se_calculate_fitness1();

                    if (fitness1 < current_best_fitness1) {
                        // Store current best.
                        current_best_fitness1 = fitness1;
                        current_best_posisions = position_indices;
                    }
                }
            }

            fitness1 = current_best_fitness1;
            position_indices = current_best_posisions;
        }

        void try_best2() {
            std::float64_t current_best_fitness1 = fitness1;
            auto current_best_posisions = position_indices;

            for (size_t i = 0; i < position_indices.size() - 1; i++) {
                std::swap(position_indices[i], position_indices[i + 1]);
                se_calculate_fitness1();

                if (fitness1 < current_best_fitness1) {
                    // Store current best.
                    current_best_fitness1 = fitness1;
                    current_best_posisions = position_indices;
                }
            }

            fitness1 = current_best_fitness1;
            position_indices = current_best_posisions;
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    reverse();
                    break;
                case 1:
                    just_swap();
                    break;
                case 2:
                    shift_left();
                    break;
                case 3:
                    shift_right();
                    break;
                case 4:
                    try_best1();
                    break;
                default:
                    try_best2();
                    break;
            }
        }

        void se_randomize() override {
            global_rng.shuffle(position_indices);
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;

            size_t last = *(position_indices.end() - 1);
            auto [x0, y0] = global_positions[last];

            for (size_t index: position_indices) {
                auto [x1, y1] = global_positions[index];
                fitness1 += hypot(x0 - x1, y0 - y1);
                x0 = x1;
                y0 = y1;
            }
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<TSPIndividual> result = std::make_unique<TSPIndividual>();
            result->position_indices = position_indices;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_numbers = tao::json::empty_array;
            tao::json::value json_tuple;

            for (size_t index: position_indices) {
                json_numbers.get_array().push_back(index);
            }

            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"position_indices", json_numbers},
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();

            const auto& arr1 = restored_json["position_indices"].get_array();

            for (size_t i = 0; i < position_indices.size(); i++) {
                position_indices[i] = arr1[i].as<size_t>();
            }
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            std::unique_ptr<TSPIndividual> result = std::make_unique<TSPIndividual>();
            result->init_positions();
            const TSPIndividual* const other_individual = dynamic_cast<const TSPIndividual* const>(individual);
            auto [i1, i2] = get_two_indices();

            if (i1 > i2) {
                std::swap(i1, i2);
            }

            size_t i = 0, j = 0;
            size_t new_index = 0;
            size_t positions_size = global_positions.size();
            std::vector<bool> used(positions_size, false);

            for (i = i1; i < i2; i++) {
                // Copy from current individual:
                new_index = position_indices[i];
                result->position_indices[i] = new_index;
                used[new_index] = true;
            }

            // Where to store indices from other individual.
            // Starting at:
            j = i2;

            for (i = 0; i < positions_size; i++) {
                new_index = other_individual->position_indices[i];

                if (!used[new_index]) {
                    // Now only copy from the other individual if index is not already used:
                    result->position_indices[j] = new_index;
                    used[new_index] = true;

                    j++;

                    if (j >= positions_size) {
                        j = 0;
                    }
                }
            }

            return result;
        }
};
