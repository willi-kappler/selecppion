/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the neuralnet3 example

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
uint32_t global_batch_counter;

class NeuralNet3Individual: public SEIndividual {
    public:
        size_t input_size_local;
        size_t output_size_local;
        const uint16_t new_neuron_prob;
        const uint16_t new_connection_prob;
        const uint16_t mutate_neuron_type_prob;
        const uint16_t max_network_size;
        std::vector<Neuron> hidden_layer;

        NeuralNet3Individual(size_t input_size, size_t output_size):
        SEIndividual(),
        input_size_local(input_size),
        output_size_local(output_size),
        new_neuron_prob(10),
        new_connection_prob(10),
        mutate_neuron_type_prob(10),
        max_network_size(10),
        hidden_layer()
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

        void evaluate_log(std::span<const std::float64_t> input_values, std::shared_ptr<spdlog::logger> se_logger) {
            se_logger->debug("NeuralNet3Individual::evaluate_log()");

            // First reset all neurons:
            for (Neuron &neuron: hidden_layer) {
                neuron.reset_value();
            }

            // Evaluate the whole network two times:
            for (uint8_t i = 0; i < 2; i++) {
                se_logger->debug("evaluate iteration: {}", i);
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

        std::float64_t calculate_error_log(std::span<const std::float64_t> expected_output, std::shared_ptr<spdlog::logger> se_logger) {
            se_logger->debug("NeuralNet3Individual::calculate_error_log()");
            std::float64_t error = 0.0;
            std::float64_t value = 0.0;

            // First n neurons are ouput neurons:
            // (n = output_size_local)
            for (size_t i = 0; i < output_size_local; i++) {
                se_logger->debug("iteration: {}", i);
                value = expected_output[i] - hidden_layer[i].current_value;
                se_logger->debug("value: {}, expected_output: {}, current_value: {}", value, expected_output[i], hidden_layer[i].current_value);
                se_logger->debug("error: {}", error);
                error += value * value;
                se_logger->debug("error: {}", error);
            }

            return error;
        }

        std::float64_t calculate_error2(std::span<const std::float64_t> expected_output) {
            std::float64_t error = 0.0;
            std::float64_t value = 0.0;
            std::float64_t max_value = 0.0;

            // First n neurons are ouput neurons:
            // (n = output_size_local)
            for (size_t i = 0; i < output_size_local; i++) {
                value = hidden_layer[i].current_value;
                if (value > max_value) {
                    max_value = value;
                }
            }

            if (max_value == 0.0) {
                // Do not divide by zero!
                max_value = 1.0;
            }

            for (size_t i = 0; i < output_size_local; i++) {
                // Scale values between 0.0 and 1.0:
                value = hidden_layer[i].current_value / max_value;
                value = expected_output[i] - value;
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

        std::float64_t evaluate_with_error_log(std::span<const std::float64_t> input_values, std::span<const std::float64_t> expected_outputs, std::shared_ptr<spdlog::logger> se_logger) {
            se_logger->debug("NeuralNet2::Individual::evaluate_with_error_log()");
            // evaluate_log(input_values, se_logger);
            evaluate(input_values);
            return calculate_error_log(expected_outputs, se_logger);
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

        void add_neuron2() {
            size_t current_size = hidden_layer.size();
            Neuron new_neuron = Neuron();

            for (size_t i = 0; i < current_size; i++) {
                // Add all hidden connections:
                new_neuron.add_hidden_connection(i);
                hidden_layer[i].add_hidden_connection(current_size);
            }

            for (size_t i = 0; i < input_size_local; i++) {
                // Add all input connections:
                new_neuron.add_input_connection(i);
            }

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
                    index2 = global_rng.get_size_t(input_size_local);
                    neuron.replace_input_connection(index2);
                    break;
                case 4:
                    index2 = global_rng.get_size_t(current_size);
                    if (index2 == index1) {
                        neuron.mutate_hidden_connection();
                    } else {
                        neuron.replace_hidden_connection(index2);
                    }
                    break;
                case 5:
                    // Split neuron:
                    if (current_size < max_network_size) {
                        Neuron new_neuron = neuron.split_neuron();
                        // Add new neuron to the network:
                        hidden_layer.push_back(new_neuron);

                        // Add one connection from a random neuron to the new neuron:
                        index2 = global_rng.get_size_t(current_size);
                        // The new neuon will be placed at the last index:
                        hidden_layer[index2].add_hidden_connection(current_size);
                    } else {
                        neuron.mutate_hidden_connection();
                    }
                    break;
                case 6:
                    // Prune one random neuron:
                    neuron.prune_connections();
                    break;
                case 7:
                    // Enhance one random neuron:
                    neuron.enhance_connections();
                    break;
                case 8:
                    // Prune all neurons:
                    for (Neuron &n: hidden_layer) {
                        n.prune_connections();
                    }
                    break;
                case 9:
                    // Enhance all neurons:
                    for (Neuron &n: hidden_layer) {
                        n.enhance_connections();
                    }
                    break;
                case 10:
                    // Add neuron:
                    prob = global_rng.get_uint16(new_neuron_prob);
                    if ((prob == 0) && (hidden_layer.size() < max_network_size)) {
                        add_neuron1();
                    } else {
                        neuron.mutate_hidden_connection();
                    }
                    break;
                case 11:
                    {
                        // Swap neurons:
                        const auto [i1, i2] = global_rng.get_two_size_t(current_size);

                        if (hidden_layer[i1].is_empty() || hidden_layer[i2].is_empty()) {
                            neuron.mutate_hidden_connection();
                        } else {
                            std::swap(hidden_layer[i1], hidden_layer[i2]);
                        }
                    }
                    break;
                case 12:
                    {
                        // Optimize bias:
                        std::float64_t current_bias = neuron.get_bias();
                        std::float64_t current_fitness1 = fitness1;
                        std::float64_t new_bias = -2.0;

                        while (new_bias <= 2.0) {
                            neuron.set_bias(new_bias);
                            se_calculate_fitness1();

                            if (fitness1 < current_fitness1) {
                                current_fitness1 = fitness1;
                                current_bias = new_bias;
                            }

                            new_bias += 0.05;
                        }

                        neuron.set_bias(current_bias);
                    }
                    break;
                case 13:
                    {
                        // Optimize one input connection:
                        if (neuron.is_empty()) {
                            neuron.mutate_input_connection();
                        } else {
                            const size_t index = neuron.get_random_input_index();
                            std::float64_t current_weight = neuron.get_input_weight(index);
                            std::float64_t current_fitness1 = fitness1;
                            std::float64_t new_weight = -2.0;

                            while (new_weight <= 2.0) {
                                neuron.set_input_weight(index, new_weight);
                                se_calculate_fitness1();

                                if (fitness1 < current_fitness1) {
                                    current_fitness1 = fitness1;
                                    current_weight = new_weight;
                                }

                                new_weight += 0.05;
                            }

                            neuron.set_input_weight(index, current_weight);
                        }
                    }
                    break;
                case 14:
                    {
                        // Optimize one hidden connection:
                        if (neuron.is_empty()) {
                            neuron.mutate_hidden_connection();
                        } else {
                            const size_t index = neuron.get_random_hidden_index();
                            std::float64_t current_weight = neuron.get_hidden_weight(index);
                            std::float64_t current_fitness1 = fitness1;
                            std::float64_t new_weight = -2.0;

                            while (new_weight <= 2.0) {
                                neuron.set_hidden_weight(index, new_weight);
                                se_calculate_fitness1();

                                if (fitness1 < current_fitness1) {
                                    current_fitness1 = fitness1;
                                    current_weight = new_weight;
                                }

                                new_weight += 0.05;
                            }

                            neuron.set_hidden_weight(index, current_weight);
                        }
                    }
                    break;
                case 15:
                    // Mutate neuron type:
                    prob = global_rng.get_uint16(mutate_neuron_type_prob);
                    if (prob == 0) {
                        neuron.mutate_neuron_type();
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

        void calculate_fitness1_log(std::shared_ptr<spdlog::logger> se_logger) {
            se_logger->debug("NeuralNet3Individual::calculate_fitness1_log()");
            std::float64_t log_fitness1 = 0.0;
            const uint16_t batch_size = global_data_provider.batch_size;

            for (uint16_t i = 0; i < batch_size; i++) {
                se_logger->debug("iteration: {}", i);
                se_logger->debug("fitness1: {}", log_fitness1);
                log_fitness1 += evaluate_with_error_log(global_data_provider.train_input_values[i], global_data_provider.train_output_values[i], se_logger);
                se_logger->debug("fitness1: {}",log_fitness1);
            }

            log_fitness1 = log_fitness1 / static_cast<std::float64_t>(batch_size);
            se_logger->debug("fitness1: {}", log_fitness1);
        }

        void se_calculate_fitness2() override {
            fitness2 = 0.0;
            const size_t test_size = global_data_provider.test_input_values.size();

            for (uint16_t i = 0; i < test_size; i++) {
                fitness2 += evaluate_with_error(global_data_provider.test_input_values[i], global_data_provider.test_output_values[i]);
            }

            fitness2 = fitness2 / static_cast<std::float64_t>(test_size);
            fitness2 = (fitness1 + fitness2) / 2.0;
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<NeuralNet3Individual> result = std::make_unique<NeuralNet3Individual>(input_size_local, output_size_local);
            result->hidden_layer = hidden_layer;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            nlohmann::json json_array = nlohmann::json::array();

            uint32_t num_connections = 0;

            for (Neuron neuron: hidden_layer) {
                json_array.push_back(neuron.to_json());
                num_connections += neuron.total_connections();
            }

            const nlohmann::json json_data = {
                {"neurons", hidden_layer.size()},
                {"connections", num_connections},
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
                global_batch_counter = 0;
                global_data_provider.set_new_training_batch();
            }
        }
/*
        void se_current_iteration(size_t iteration, uint32_t num_of_iterations) override {
            // This method is only called for the individual at index 0.

            // Avoid overfitting by randomly changeing the training batch:
            if (num_of_iterations - iteration > 2000) {
                global_batch_counter++;

                if (global_batch_counter > 1000) {
                    global_batch_counter = 0;
                    global_data_provider.set_new_training_batch();
                }
            }
        }
*/
        std::unique_ptr<SEIndividual> se_crossover(const SEIndividual* const individual) override {
            [[maybe_unused]] const NeuralNet3Individual* const other_individual = dynamic_cast<const NeuralNet3Individual* const>(individual);
            std::unique_ptr<NeuralNet3Individual> result = std::make_unique<NeuralNet3Individual>(input_size_local, output_size_local);

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
