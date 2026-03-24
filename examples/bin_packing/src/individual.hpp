/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the bin packing example

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
        std::float64_t capacity_per_bin;
        std::vector<std::vector<std::float64_t>> bins;

        BinPackingIndividual(std::vector<std::float64_t> initial_items, std::float64_t initial_capacity):
        items(initial_items),
        capacity_per_bin(initial_capacity),
        bins()
        {}

        bool does_fit(std::float64_t item, size_t index) {
            std::float64_t sum = item;

            for (std::float64_t v: bins[index]) {
                sum += v;
            }

            return sum <= capacity_per_bin;
        }

        void swap_items() {
            // TODO
            const size_t num_of_bins = bins.size();

            if (num_of_bins <= 1) {
                return;
            }

            while (true) {
                auto [i1, i2] = global_rng.get_two_size_t(num_of_bins);
                size_t size1 = bins[i1].size();
                size_t size2 = bins[i2].size();

                if ((size1 > 0) && (size2 > 0)) {
                    size_t j1 = global_rng.get_size_t(size1);
                    size_t j2 = global_rng.get_size_t(size2);
                    std::swap(bins[i1][j1], bins[i2][j2]);
                }
            }
        }

        void move_item() {
            const size_t num_of_bins = bins.size();

            if (num_of_bins <= 1) {
                return;
            }

            while (true) {
                auto [i1, i2] = global_rng.get_two_size_t(num_of_bins);

                if (bins[i1].size() > 0) {
                    size_t j1 = global_rng.get_size_t(bins[i1].size());
                    // Get item from current bin:
                    std::float64_t item = bins[i1][j1];
                    // And remove it from its bin:
                    bins[i1][j1] = bins[i1].back();
                    bins[i1].pop_back();
                    // Put it into the destination bin:
                    bins[i2].push_back(item);

                    if (bins[i1].size() == 0) {
                        // Remove this empty bin:
                        bins[i1] = bins.back();
                        bins.pop_back();
                    }

                    break;
                }
            }
        }

        void move_item_old() {
            // TODO
            const size_t num_of_bins = bins.size();

            if (num_of_bins <= 1) {
                return;
            }

            while (true) {
                size_t i1 = global_rng.get_size_t(num_of_bins);
                if (bins[i1].size() > 0) {
                    size_t j1 = global_rng.get_size_t(bins[i1].size());
                    // Get item from current bin:
                    std::float64_t item = bins[i1][j1];
                    // And remove it from its bin:
                    bins[i1][j1] = bins[i1].back();
                    bins[i1].pop_back();

                    if (bins[i1].size() == 0) {
                        // Remove this empty bin:
                        bins[i1] = bins.back();
                        bins.pop_back();
                    }

                    for (size_t i2 = 0; i2 < num_of_bins; i2++) {
                        if (i1 != i2) {
                            if (does_fit(item, i2)) {
                                bins[i2].push_back(item);
                                return;
                            }
                        }
                    }

                    // Does not fit in any other bin, so create a new one:
                    bins.push_back(std::vector<std::float64_t>(item));
                    break;
                }
            }
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    swap_items();
                    break;
                default:
                    move_item();
                    break;
            }
        }

        void se_randomize() override {
            const size_t num_items = items.size();
            std::vector<std::float64_t> selection = std::vector<std::float64_t>(0.0, num_items);

            for (size_t i = 0; i < num_items; i++) {
                selection[i] = items[i];
            }

            global_rng.shuffle(selection);

            bins.clear();
            bins.push_back(std::vector<std::float64_t>());
            size_t current_bin = 0;
            std::float64_t current_sum = 0.0;
            std::float64_t current_item = 0.0;

            while (selection.size() > 0) {
                current_item = selection.back();
                selection.pop_back();

                if (current_sum + current_item > capacity_per_bin) {
                    // Add new bin
                    bins.push_back(std::vector<std::float64_t>());
                    current_bin++;
                    current_sum = 0.0;
                }

                current_sum += current_item;
                bins[current_bin].push_back(current_item);
            }
        }

        void se_calculate_fitness1() override {
            std::float64_t sum = 0.0;
            fitness1 = bins.size();

            for (auto bin: bins) {
                sum = 0.0;

                for (auto v: bin) {
                    sum += v;
                }

                if (sum > capacity_per_bin) {
                    // Add penalty:
                    fitness1 += 100.0;
                }
            }
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<BinPackingIndividual> result = std::make_unique<BinPackingIndividual>(items, capacity_per_bin);

            result->bins = bins;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_arrays = tao::json::empty_array;
            tao::json::value json_numbers = tao::json::empty_array;

            for (auto bin: bins) {
                json_numbers.get_array().clear();

                for (auto v: bin) {
                    json_numbers.get_array().push_back(double(v));
                }

                json_arrays.get_array().push_back(json_numbers);
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

            // TODO
            // const auto& arr1 = restored_json["bins"].get_array();
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            const BinPackingIndividual* const other_individual = dynamic_cast<const BinPackingIndividual* const>(individual);
            std::unique_ptr<BinPackingIndividual> result = std::make_unique<BinPackingIndividual>(items, capacity_per_bin);

            // TODO
            result->bins = other_individual->bins;

            return result;
        }
};
