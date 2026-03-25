/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the knapsack example

    To just build use:
    xmake build se_example_knapsack

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

class KnapSackIndividual: public SEIndividual {
    public:
        std::vector<std::float64_t> items;
        std::float64_t total_capacity;
        std::vector<uint8_t> selection;

        KnapSackIndividual(std::vector<std::float64_t> initial_items, std::float64_t initial_capacity):
        items(initial_items),
        total_capacity(initial_capacity),
        selection(std::vector<uint8_t>(initial_items.size(), 0))
        {}

        void swap_items() {
            const size_t num_of_items = selection.size();
            auto [i1, i2] = global_rng.get_two_size_t(num_of_items);
            std::swap(selection[i1], selection[i2]);
        }

        void select_item() {
            size_t i = global_rng.get_size_t(selection.size());
            selection[i] = 1;
        }

        void unselect_item() {
            size_t i = global_rng.get_size_t(selection.size());
            selection[i] = 0;
        }

        void unselect_all() {
            for (uint8_t &s: selection) {
                s = 0;
            }
        }

        void select_all() {
            for (uint8_t &s: selection) {
                s = 1;
            }
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    swap_items();
                    break;
                case 1:
                    select_item();
                    break;
                case 2:
                    unselect_item();
                    break;
                case 3:
                    unselect_all();
                    break;
                default:
                    select_all();
                    break;
            }
        }

        void se_randomize() override {
            for (uint8_t &s: selection) {
                // In range [0, 1, 2[
                s = global_rng.get_uint8(2);
            }
        }

        void se_calculate_fitness1() override {
            const size_t num_of_items = items.size();
            std::float64_t sum = 0.0;
            uint32_t item_count = 0;

            for (size_t i = 0; i < num_of_items; i++) {
                sum += (items[i] * selection[i]);
                item_count += selection[i];
            }

            fitness1 = num_of_items - item_count;

            if (sum > total_capacity) {
                // Penalty if capacity exceeded:
                fitness1 += 100.0;
            }
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<KnapSackIndividual> result = std::make_unique<KnapSackIndividual>(items, total_capacity);

            result->selection = selection;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_arrays = tao::json::empty_array;

            for (uint8_t s: selection) {
                json_arrays.get_array().push_back(s);
            }

            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"bins", json_arrays}
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();

            const auto& arr1 = restored_json["bins"].get_array();

            for (size_t i = 0; i < selection.size(); i++) {
                selection[i] = arr1[i].as<uint8_t>();
            }

        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            const KnapSackIndividual* const other_individual = dynamic_cast<const KnapSackIndividual* const>(individual);
            std::unique_ptr<KnapSackIndividual> result = std::make_unique<KnapSackIndividual>(items, total_capacity);

            const size_t num_of_items = items.size();
            auto [i1, i2] = global_rng.get_two_size_t(num_of_items);

            for (size_t i = 0; i < num_of_items; i++) {
                if ((i >= i1) && (i < i2)) {
                    result->selection[i] = selection[i];
                } else {
                    result->selection[i] = other_individual->selection[i];
                }
            }

            return result;
        }
};
