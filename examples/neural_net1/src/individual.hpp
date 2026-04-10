/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the neuralnet1 example

    To just build use:
    xmake build se_example_neuralnet1

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

using namespace secpion;

SE_RNG_L64 global_rng;

class NeuralNet1Individual: public SEIndividual {
    public:
        NeuralNet1Individual()
        {
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
            std::unique_ptr<NeuralNet1Individual> result = std::make_unique<NeuralNet1Individual>();

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_arrays = tao::json::empty_array;

            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
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
            [[maybe_unused]] const NeuralNet1Individual* const other_individual = dynamic_cast<const NeuralNet1Individual* const>(individual);
            std::unique_ptr<NeuralNet1Individual> result = std::make_unique<NeuralNet1Individual>();

            return result;
        }
};
