/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the rastrigin example

    To just build use:
    xmake build se_example_rastrigin

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_random.hpp"
#include "secpion/se_individual.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

class RastriginIndividual: public SEIndividual {
    public:
        size_t dimensions;
        std::float64_t lower_bound;
        std::float64_t upper_bound;
        std::vector<std::float64_t> values;

        RastriginIndividual(size_t dimensions, std::float64_t lower_bound, std::float64_t upper_bound):
        dimensions(dimensions),
        lower_bound(lower_bound),
        upper_bound(upper_bound),
        values(std::vector<std::float64_t>(dimensions, 0))
        {
            for (size_t i = 0; i < dimensions; i++) {
                values[i] = global_rng.get_float64(lower_bound, upper_bound);
            }
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                default:
                    break;
            }
        }

        void se_randomize() override {
        }

        void se_calculate_fitness1() override {
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<RastriginIndividual> result = std::make_unique<RastriginIndividual>(dimensions, lower_bound, upper_bound);

            result->values = values;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_arrays = tao::json::empty_array;

            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"values", json_arrays}
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            const RastriginIndividual* const other_individual = dynamic_cast<const RastriginIndividual* const>(individual);
            std::unique_ptr<RastriginIndividual> result = std::make_unique<RastriginIndividual>(dimensions, lower_bound, upper_bound);

            auto [i1, i2] = global_rng.get_two_size_t(dimensions);

            for (size_t i = 0; i < dimensions; i++) {
                if ((i >= i1) && (i <= i2)) {
                    result->values[i] = values[i];
                } else {
                    result->values[i] = other_individual->values[i];
                }
            }

            return result;
        }
};
