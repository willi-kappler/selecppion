/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the TSP example

    To just build use:
    xmake build se_example_bin_packing

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

class BinPackingIndividual: public SEIndividual {
    public:
        std::vector<std::float64_t> items;
        std::vector<uint32_t> selection;
        std::float64_t capacity;
        std::float64_t final_penalty;

        BinPackingIndividual(std::vector<std::float64_t> initial_items, std::float64_t initial_capacity):
        items(initial_items),
        selection(),
        capacity(initial_capacity),
        final_penalty()
        {}

        void init() {
            se_randomize();
            reset_penalty();
        }

        void reset_penalty() {
            final_penalty = 0.0;

            for (auto v: items) {
                final_penalty += v;
            }
        }

        void inc_bin() {
            size_t i = global_rng.get_size_t(items.size());
            selection[i]++;
        }

        void dec_bin() {
            size_t i = global_rng.get_size_t(items.size());

            if (selection[i] > 0) {
                selection[i]++;
            }
        }

        void set_to_one() {
            size_t i = global_rng.get_size_t(items.size());
            selection[i] = 1;
        }

        void swap() {
            global_rng.swap(selection);
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    inc_bin();
                    break;
                case 1:
                    dec_bin();
                    break;
                case 2:
                    set_to_one();
                    break;
                default:
                    swap();
                    break;
            }
        }

        void se_randomize() override {
            selection.assign(items.size(), 0);

            for (auto &item: selection) {
                item = global_rng.get_uint8(10);
            }
        }

        void se_calculate_fitness1() override {
        }

        std::float64_t se_actual_fitness() override {
            return final_penalty - fitness1;
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<BinPackingIndividual> result = std::make_unique<BinPackingIndividual>(items, capacity);
            result->selection = selection;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_numbers = tao::json::empty_array;
            tao::json::value json_tuple;

            const tao::json::value json_data;

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();
        }
};
