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

#include "neuron.hpp"

using namespace secpion;

class NeuralNet1Individual: public SEIndividual {
    public:
        size_t input_size_local;
        size_t output_size_local;
        const uint16_t new_node_prob = 10000;
        const uint16_t new_connection_prob = 100;
        std::vector<Neuron> hidden_layer;

        NeuralNet1Individual(size_t input_size, size_t output_size):
            input_size_local(input_size),
            output_size_local(output_size),
            hidden_layer()
        {
            se_randomize();
        }

        void add_neuron() {
            size_t current_size = hidden_layer.size();
            Neuron new_neuron = Neuron();

            // Add a random connextion to the neuron:
            size_t index = global_rng.get_size_t(current_size);
            new_neuron.add_hidden_connection(index);

            // Add a connection from this new neuron to a random existing neuron:
            index = global_rng.get_size_t(current_size);
            hidden_layer[index].add_hidden_connection(current_size);

            // Finally add the neuron to the hidden layer:
            hidden_layer.push_back(new_neuron);
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
            hidden_layer.clear();

            for (uint16_t i = 0; i < output_size_local; i++) {
                hidden_layer.push_back(Neuron());
            }

            // Start with two neurons:
            add_neuron();
            add_neuron();

            // Two connection to the first input layer:
            size_t current_size = hidden_layer.size();

            hidden_layer[current_size - 2].add_input_connection(0);
            hidden_layer[current_size - 2].add_input_connection(1);
            hidden_layer[current_size - 1].add_input_connection(0);
            hidden_layer[current_size - 1].add_input_connection(1);
        }

        void se_calculate_fitness1() override {
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<NeuralNet1Individual> result = std::make_unique<NeuralNet1Individual>(input_size_local, output_size_local);

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
            std::unique_ptr<NeuralNet1Individual> result = std::make_unique<NeuralNet1Individual>(input_size_local, output_size_local);

            return result;
        }
};
