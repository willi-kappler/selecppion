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
#include <memory>
#include <stdexcept>

// External includes:
#include <spdlog/spdlog.h>

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
        const uint16_t max_network_size;
        std::vector<Neuron> hidden_layer;
        std::string local_metadata;

        NeuralNet2Individual(size_t input_size, size_t output_size):
        SEIndividual(),
        input_size_local(input_size),
        output_size_local(output_size),
        new_node_prob(10),
        new_connection_prob(10),
        max_network_size(10),
        hidden_layer(),
        local_metadata()
        {
            se_randomize();
        }

        void evaluate(std::span<const std::float64_t> input_values) {
            // First reset all neurons:
            for (Neuron &neuron: hidden_layer) {
                neuron.reset_value();
            }

            // Evaluate the whole network two times:
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
            std::float64_t value = 0.0;

            // First n neurons are ouput neurons:
            // (n = output_size_local)
            for (size_t i = 0; i < output_size_local; i++) {
                value = expected_output[i] - hidden_layer[i].current_value;
                error += value * value;
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

        void add_neuron1() {
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

        void se_mutate(uint8_t op) override {
            const size_t current_size = hidden_layer.size();
            const size_t index1 = global_rng.get_size_t(current_size);
            size_t index2 = 0;
            Neuron &neuron = hidden_layer[index1];
            uint16_t prob = 0;

            switch (op) {
                case 0:
                    neuron.mutate_bias();
                    break;
                case 1:
                    // Add input connection:
                    prob = global_rng.get_uint16(new_connection_prob);
                    if (prob == 0) {
                        index2 = global_rng.get_size_t(input_size_local);
                        neuron.add_input_connection(index2);
                    } else {
                        neuron.mutate_input_connection();
                    }
                    break;
                case 2:
                    // Add hidden connection:
                    prob = global_rng.get_uint16(new_connection_prob);
                    if (prob == 0) {
                        index2 = global_rng.get_size_t(current_size);
                        if (index2 == index1) {
                            neuron.mutate_hidden_connection();
                        } else {
                            neuron.add_hidden_connection(index2);
                        }
                    } else {
                        neuron.mutate_hidden_connection();
                    }
                    break;
                case 3:
                    // Add neuron:
                    prob = global_rng.get_uint16(new_node_prob);
                    if ((prob == 0) && (hidden_layer.size() < max_network_size)) {
                        add_neuron1();
                    } else {
                        neuron.mutate_hidden_connection();
                    }
                    break;
                default:
                    throw std::runtime_error(fmt::format("Unknown mutation operation: {}", op));
                    break;
            }
        }

        void se_randomize() override {
            hidden_layer.clear();
            // Add the output neurons to the network:
            for (uint16_t i = 0; i < output_size_local; i++) {
                hidden_layer.push_back(Neuron());
            }

            // Start with one normal neuron (at least one is needed):
            add_neuron1();
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;

            const uint16_t batch_size = global_data_provider.batch_size;

            for (uint16_t i = 0; i < batch_size; i++) {
                fitness1 += evaluate_with_error(global_data_provider.train_input_values[i], global_data_provider.train_output_values[i]);
            }

            fitness1 = fitness1 / static_cast<std::float64_t>(batch_size);
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() const override {
            std::unique_ptr<NeuralNet2Individual> result = std::make_unique<NeuralNet2Individual>(input_size_local, output_size_local);
            result->hidden_layer = hidden_layer;

            return result;
        }

        void se_copy_from(const SEIndividual &individual) override {
            const NeuralNet2Individual& neural_individual = dynamic_cast<const NeuralNet2Individual&>(individual);
            hidden_layer = neural_individual.hidden_layer;
        }


        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            nlohmann::json json_array = nlohmann::json::array();

            for (Neuron neuron: hidden_layer) {
                json_array.push_back(neuron.to_json());
            }

            const nlohmann::json json_data = {
                {"neurons", hidden_layer.size()},
                {"connections", num_of_connections()},
                {"fitness1", double(fitness1)},
                {"fitness2", double(fitness2)},
                {"hidden_layer", json_array},
                {"metadata", local_metadata}
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

            local_metadata = restored_json["metadata"].get<std::string>();
        }

        void se_reseed_rng(size_t index) override {
            if (index == 0) {
                global_rng.seed();
            }
        }

        void se_new_best_individual(std::shared_ptr<spdlog::logger> se_logger) override {
            se_logger->debug("----- Network size: {}, num of connections: {}", hidden_layer.size(), num_of_connections());
        }

        void se_set_metadata(std::string metadata) override {
            local_metadata = metadata;
        }

        std::string se_get_metadata() override {
            return local_metadata;
        }

        uint32_t num_of_connections() {
            uint32_t num_connections = 0;

            for (Neuron neuron: hidden_layer) {
                num_connections += neuron.total_connections();
            }

            return num_connections;
        }
};
