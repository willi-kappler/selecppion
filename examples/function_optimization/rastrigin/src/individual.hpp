/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the rastrigin example

    To just build use:
    xmake build se_example_rastrigin

    Run with:
    ./run_example.sh
*/

// STD includes:
#include <cmath>
#include <numbers>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"
#include "secpion/se_utils.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

class RastriginIndividual: public SEIndividual {
    public:
        size_t local_dimensions;
        std::float64_t local_lower_bound;
        std::float64_t local_upper_bound;
        std::vector<std::float64_t> values;

        RastriginIndividual(size_t dimensions, std::float64_t lower_bound, std::float64_t upper_bound):
        local_dimensions(dimensions),
        local_lower_bound(lower_bound),
        local_upper_bound(upper_bound),
        values(std::vector<std::float64_t>(dimensions, 0))
        {
            se_randomize();
        }

        void inc_value() {
            size_t i = global_rng.get_size_t(local_dimensions);
            values[i] += global_rng.get_float64();
            if (values[i] > local_upper_bound) {
                values[i] = local_upper_bound;
            }
        }

        void dec_value() {
            size_t i = global_rng.get_size_t(local_dimensions);
            values[i] += global_rng.get_float64();
            if (values[i] < local_lower_bound) {
                values[i] = local_lower_bound;
            }
        }

        void random_value1() {
            size_t i = global_rng.get_size_t(local_dimensions);
            values[i] = global_rng.get_float64(local_lower_bound, local_upper_bound);
        }

        void random_value2() {
            size_t i = global_rng.get_size_t(local_dimensions);
            values[i] = global_rng.get_float64(local_lower_bound, local_upper_bound);
        }

        void all_equal() {
            std::float64_t v = global_rng.get_float64(local_lower_bound, local_upper_bound);
            for (size_t i = 0; i < local_dimensions; i++) {
                values[i] = v;
            }
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    inc_value();
                    break;
                case 1:
                    dec_value();
                    break;
                case 2:
                    random_value1();
                    break;
                case 3:
                    random_value2();
                    break;
                default:
                    all_equal();
                    break;
            }
        }

        void se_randomize() override {
            for (size_t i = 0; i < local_dimensions; i++) {
                values[i] = global_rng.get_float64(local_lower_bound, local_upper_bound);
            }
        }

        void se_calculate_fitness1() override {
            const std::float64_t A = 10.0;
            std::float64_t fitness = A * local_dimensions;
            std::float64_t term1, term2;

            for (size_t i = 0; i < local_dimensions; i++) {
                term1 = pow(values[i], 2.0);
                term2 = A * cos(2.0 * std::numbers::pi * values[i]);
                fitness += term1 - term2;
            }

            fitness1 = fitness;
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<RastriginIndividual> result = std::make_unique<RastriginIndividual>(local_dimensions, local_lower_bound, local_upper_bound);

            result->values = values;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"values", se_vec_to_json<std::float64_t>(values)}
            };

            return se_json_to_vec_u8(json_data);
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            tao::json::value restored_json = se_span_u8_to_json(data);
            fitness1 = restored_json["fitness1"].as<double>();
            se_json_to_vec(restored_json["values"], values);
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            const RastriginIndividual* const other_individual = dynamic_cast<const RastriginIndividual* const>(individual);
            std::unique_ptr<RastriginIndividual> result = std::make_unique<RastriginIndividual>(local_dimensions, local_lower_bound, local_upper_bound);

            auto [i1, i2] = global_rng.get_two_size_t(local_dimensions);

            for (size_t i = 0; i < local_dimensions; i++) {
                if ((i >= i1) && (i <= i2)) {
                    result->values[i] = values[i];
                } else {
                    result->values[i] = other_individual->values[i];
                }
            }

            return result;
        }
};
