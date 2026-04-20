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

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"
#include "secpion/se_utils.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

class BinPackingIndividual: public SEIndividual {
    public:
        std::vector<std::float64_t> items;
        // This contains a mapping: which items are stored in which bin.
        // [12.3, 8.6, 1.5, 9.9, 22.47]
        // [0,    0,   1,   2,   1    ]
        // Bin 0 contains 12.3 and 8.6
        // Bin 1 contains 1.5 and 22.47
        // Bin 2 contains 9.9
        std::float64_t capacity_per_bin;
        std::vector<size_t> bins;

        BinPackingIndividual(std::vector<std::float64_t> initial_items, std::float64_t initial_capacity):
        items(initial_items),
        capacity_per_bin(initial_capacity),
        bins(std::vector<size_t>(initial_items.size(), 0))
        {}

        void swap_items() {
            const size_t num_of_items = bins.size();
            auto [i1, i2] = global_rng.get_two_size_t(num_of_items);
            std::swap(bins[i1], bins[i2]);
        }

        void move_bin_up() {
            size_t i = global_rng.get_size_t(bins.size());
            bins[i]++;
        }

        void move_bin_down() {
            size_t i = global_rng.get_size_t(bins.size());
            if (bins[i] > 0) {
                bins[i]--;
            }
        }

        void set_to_zero() {
            for (size_t &b: bins) {
                b = 0;
            }
        }

        void set_ramp() {
            for (size_t i = 0; i < bins.size(); i++) {
                bins[i] = i;
            }
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    swap_items();
                    break;
                case 1:
                    move_bin_up();
                    break;
                case 2:
                    move_bin_down();
                    break;
                case 3:
                    set_to_zero();
                    break;
                default:
                    set_ramp();
                    break;
            }
        }

        void se_randomize() override {
            const size_t num_of_items = items.size();

            for (size_t &b: bins) {
                b = global_rng.get_size_t(num_of_items);
            }
        }

        void se_calculate_fitness1() override {
            std::unordered_map<size_t, std::float64_t> bin_sum;
            std::float64_t penalty = 0.0;

            for (size_t i = 0; i < items.size(); i++) {
                bin_sum[bins[i]] += items[i];
            }

            for (auto &elem: bin_sum) {
                if (elem.second > capacity_per_bin) {
                    penalty += 100.0;
                }
            }

            fitness1 = bin_sum.size() + penalty;
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<BinPackingIndividual> result = std::make_unique<BinPackingIndividual>(items, capacity_per_bin);

            result->bins = bins;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"bins", se_vec_to_json<size_t>(bins)}
            };

            return se_json_to_vec_u8(json_data);
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            tao::json::value restored_json = se_span_u8_to_json(data);
            fitness1 = restored_json["fitness1"].as<double>();
            se_json_to_vec(restored_json["bins"], bins);
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            const BinPackingIndividual* const other_individual = dynamic_cast<const BinPackingIndividual* const>(individual);
            std::unique_ptr<BinPackingIndividual> result = std::make_unique<BinPackingIndividual>(items, capacity_per_bin);

            const size_t num_of_bins = bins.size();
            auto [i1, i2] = global_rng.get_two_size_t(num_of_bins);

            for (size_t i = 0; i < num_of_bins; i++) {
                if ((i >= i1) && (i < i2)) {
                    result->bins[i] = bins[i];
                } else {
                    result->bins[i] = other_individual->bins[i];
                }
            }

            return result;
        }
};
