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

        std::float64_t test_network() {
            std::float64_t loss = 0.0;

            evaluate({0.0, 0.0});
            loss += calculate_error({0.0});
            evaluate({1.0, 0.0});
            loss += calculate_error({1.0});
            evaluate({0.0, 1.0});
            loss += calculate_error({1.0});
            evaluate({1.0, 1.0});
            loss += calculate_error({0.0});

            return loss / 4.0;
        }

        void evaluate(std::span<const std::float64_t> input_values) {
            // First reset all neurons:
            for (auto neuron: hidden_layer) {
                neuron.reset_value();
            }

            for (auto neuron: hidden_layer) {
                neuron.evaluate(input_values, hidden_layer);
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

        void evaluate_with_error(std::span<const std::float64_t> input_values, std::span<const std::float64_t> expected_outputs) {
            evaluate(input_values);
            fitness1 += calculate_error(expected_outputs);
        }

        void evaluate_with_error(std::initializer_list<std::float64_t> input_values,
            std::initializer_list<std::float64_t> expected_outputs) {
            evaluate_with_error(std::span<const std::float64_t>{input_values.begin(), input_values.end()},
                std::span<const std::float64_t>{expected_outputs.begin(), expected_outputs.end()});
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

        void swap_neurons() {
            size_t current_size = hidden_layer.size();
            size_t i1 = global_rng.get_size_t(current_size);
            size_t i2 = global_rng.get_size_t(current_size);

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
                    if (prob == 0) {
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
            fitness1 = 0.0;

            evaluate_with_error({0.0, 0.0}, {0.0});
            evaluate_with_error({1.0, 0.0}, {1.0});
            evaluate_with_error({0.0, 1.0}, {1.0});
            evaluate_with_error({0.0, 0.0}, {0.0});

            fitness1 = fitness1 / 4.0;
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<NeuralNet1Individual> result = std::make_unique<NeuralNet1Individual>(input_size_local, output_size_local);
            result->hidden_layer = hidden_layer;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_array = tao::json::empty_array;

            for (auto neuron: hidden_layer) {
                json_array.get_array().push_back(neuron.to_json());
            }

            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"hidden_layer", json_array}
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();
            Neuron n;
            hidden_layer.clear();

            for (auto item: restored_json["hidden_layer"].get_array()) {
                n.from_json(item);
                hidden_layer.push_back(n);
            }
        }

        void se_reseed_rng(size_t index) {
            if (index == 0) {
                global_rng.seed();
            }
        }

        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) {
            [[maybe_unused]] const NeuralNet1Individual* const other_individual = dynamic_cast<const NeuralNet1Individual* const>(individual);
            std::unique_ptr<NeuralNet1Individual> result = std::make_unique<NeuralNet1Individual>(input_size_local, output_size_local);

            // TODO

            return result;
        }
};
