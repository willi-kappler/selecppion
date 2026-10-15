/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the neuralnet2 example

    To just build use (from the main folder):
    ./build.sh

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

#include "neuron.hpp"
#include "data_provider.hpp"

using namespace secpion;

DataProvider global_data_provider;

class NeuralNet2Individual: public SEIndividual {
    public:
        size_t input_size_local;
        size_t output_size_local;
        const uint16_t new_node_prob;
        const uint16_t new_connection_prob;
        std::vector<Neuron> hidden_layer;

        NeuralNet2Individual(size_t input_size, size_t output_size):
        SEIndividual(),
        input_size_local(input_size),
        output_size_local(output_size),
        new_node_prob(10000),
        new_connection_prob(100),
        hidden_layer()
        {
            se_randomize();
        }

        void evaluate(std::span<const std::float64_t> input_values) {
            // First reset all neurons:
            for (Neuron &neuron: hidden_layer) {
                neuron.reset_value();
            }

            // Evaluate the network two times:
            for (uint8_t i = 0; i < 2; i++) {
                for (Neuron &neuron: hidden_layer) {
                    neuron.evaluate(input_values, hidden_layer);
                }
            }
        }

        void evaluate(std::initializer_list<std::float64_t> input_values) {
            evaluate(std::span<const std::float64_t>{input_values.begin(), input_values.end()});
        }

        std::float64_t calculate_error(std::span<const std::float64_t> expected_output) {
            std::float64_t error = 0.0;

            // The first n neurons are output neurons:
            for (size_t i = 0; i < output_size_local; i++) {
                error += pow(expected_output[i] - hidden_layer[i].current_value, 2.0);
            }

            return error;
        }

        std::float64_t calculate_error(std::initializer_list<std::float64_t> expected_values) {
            return calculate_error(std::span<const std::float64_t>{expected_values.begin(), expected_values.end()});
        }

        std::float64_t evaluate_with_error(std::span<const std::float64_t> input_values, std::span<const std::float64_t> expected_outputs) {
            evaluate(input_values);
            return calculate_error(expected_outputs);
        }

        std::float64_t evaluate_with_error(std::initializer_list<std::float64_t> input_values,
            std::initializer_list<std::float64_t> expected_outputs) {
            return evaluate_with_error(std::span<const std::float64_t>{input_values.begin(), input_values.end()},
                std::span<const std::float64_t>{expected_outputs.begin(), expected_outputs.end()});
        }

        void add_neuron() {
            size_t current_size = hidden_layer.size();
            Neuron new_neuron = Neuron();

            // Add a random connection to the neuron:
            size_t index = global_rng.get_size_t(current_size);
            new_neuron.add_hidden_connection(index);

            // Add a connection from this new neuron to a random existing neuron:
            index = global_rng.get_size_t(current_size);
            hidden_layer[index].add_hidden_connection(current_size);

            // Finally add the neuron to the hidden layer:
            hidden_layer.push_back(new_neuron);
        }

        void swap_neurons() {
            size_t current_size = hidden_layer.size();
            const auto [i1, i2] = global_rng.get_two_size_t(current_size);

            if (hidden_layer[i1].is_empty() || hidden_layer[i2].is_empty()) {
                mutate_neuron();
            } else {
                std::swap(hidden_layer[i1], hidden_layer[i2]);
            }

        }

        void mutate_neuron() {
            const size_t current_size = hidden_layer.size();
            const size_t index1 = global_rng.get_size_t(current_size);
            size_t index2 = 0;
            Neuron &neuron = hidden_layer[index1];
            const uint8_t mut_op = global_rng.get_uint8(5);
            uint16_t prob = 0;

            switch (mut_op) {
                case 0:
                    neuron.mutate_bias();
                    break;
                case 1:
                    prob = global_rng.get_uint16(new_connection_prob);
                    if (prob == 0) {
                        index2 = global_rng.get_size_t(input_size_local);
                        neuron.add_input_connection(index2);
                    } else {
                        neuron.mutate_input_connection();
                    }
                    break;
                case 2:
                    prob = global_rng.get_uint16(new_connection_prob);
                    if (prob == 0) {
                        index2 = global_rng.get_size_t(current_size);
                        neuron.add_hidden_connection(index2);
                    } else {
                        neuron.mutate_hidden_connection();
                    }
                    break;
                case 3:
                    index2 = global_rng.get_size_t(input_size_local);
                    neuron.replace_input_connection(index2);
                    break;
                case 4:
                    index2 = global_rng.get_size_t(current_size);
                    neuron.replace_hidden_connection(index2);
                    break;
            }
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0: {
                    const uint16_t prob = global_rng.get_uint16(new_node_prob);
                    if ((prob == 0) && (hidden_layer.size() < 10)) {
                        add_neuron();
                    } else {
                        mutate_neuron();
                    }
                    break;
                }
                case 1:
                    swap_neurons();
                    break;
                default:
                    mutate_neuron();
                    break;
            }
        }

        void se_randomize() override {
            hidden_layer.clear();

            for (uint16_t i = 0; i < output_size_local; i++) {
                hidden_layer.push_back(Neuron());
            }

            // Start with one neurons (at least one is needed):
            add_neuron();
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;

            for (uint16_t i = 0; i < global_data_provider.batch_size; i++) {
                fitness1 += evaluate_with_error(global_data_provider.train_input_values[i], global_data_provider.train_output_values[i]);
            }

            fitness1 = fitness1 / static_cast<std::float64_t>(global_data_provider.batch_size);
        }

        void se_calculate_fitness2() override {
            fitness2 = 0.0;
            const size_t test_size = global_data_provider.test_input_values.size();

            for (uint16_t i = 0; i < test_size; i++) {
                fitness2 += evaluate_with_error(global_data_provider.test_input_values[i], global_data_provider.test_output_values[i]);
            }

            fitness2 = fitness2 / static_cast<std::float64_t>(test_size);
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<NeuralNet2Individual> result = std::make_unique<NeuralNet2Individual>(input_size_local, output_size_local);
            result->hidden_layer = hidden_layer;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            nlohmann::json json_array = nlohmann::json::array();

            for (Neuron neuron: hidden_layer) {
                json_array.push_back(neuron.to_json());
            }

            const nlohmann::json json_data = {
                {"fitness1", double(fitness1)},
                {"fitness2", double(fitness2)},
                {"hidden_layer", json_array}
            };

            return se_json_to_vec_u8(json_data);
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            nlohmann::json restored_json = nlohmann::json::parse(data);

            fitness1 = restored_json["fitness1"].get<double>();
            fitness2 = restored_json["fitness2"].get<double>();
            Neuron n;
            hidden_layer.clear();

            for (auto item: restored_json["hidden_layer"]) {
                n.from_json(item);
                hidden_layer.push_back(n);
            }
        }

        void se_reseed_rng(size_t index) override {
            if (index == 0) {
                global_rng.seed();
            }
        }

        void se_prepare_individual(size_t index) override {
            // This is called before the iteration begins.
            if (index == 0) {
                // This needs to be done only once, for the individual at index 0.
                // They all share the same global data provider.
                // This will create a new batch of training data:
                global_data_provider.set_new_training_batch();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) override {
            [[maybe_unused]] const NeuralNet2Individual* const other_individual = dynamic_cast<const NeuralNet2Individual* const>(individual);
            std::unique_ptr<NeuralNet2Individual> result = std::make_unique<NeuralNet2Individual>(input_size_local, output_size_local);

            // TODO

            return result;
        }

        void print_network() {
            for (Neuron n: hidden_layer) {
                n.print_neuron();
                std::print("-----\n");
            }
            std::print("#####\n");
        }
};
