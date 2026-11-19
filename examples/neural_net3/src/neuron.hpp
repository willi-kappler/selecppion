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
#include <algorithm>
#include <vector>
#include <utility>
#include <span>
#include <print>
#include <memory>

// External includes:
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>


// Local includes:
#include "secpion/se_random.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

using Connection = std::pair<size_t, std::float64_t>;

[[nodiscard]] std::float64_t change_delta(std::float64_t value) {
    const uint8_t n = global_rng.get_uint8(10);

    if (n == 0) {
        return global_rng.get_float64_unif1();
    } else {
        const std::float64_t delta = global_rng.get_float64_unif2();

        value += delta;
        return std::clamp<std::float64_t>(value, -10.0, 10.0);
    }
}

enum class NeuronType: uint8_t {
    WeightSum,
    WeightProduct,
    Min,
    Max,
    EndOfEnum
};

NeuronType random_neuron_type() {
    const uint8_t limit = std::to_underlying(NeuronType::EndOfEnum);
    const uint8_t new_type = global_rng.get_uint8(limit);
    return static_cast<NeuronType>(new_type);
}

class Neuron {
    private:
        std::vector<Connection> input_connections;
        std::vector<Connection> hidden_connections;
        std::float64_t bias;
        NeuronType neuron_type;

    public:
        uint8_t current_mut_op;
        std::float64_t current_value;

        Neuron():
            input_connections(),
            hidden_connections(),
            bias(global_rng.get_float64_unif1()),
            neuron_type(random_neuron_type()),
            current_mut_op(0),
            current_value(0.0)
        {}

        void reset_value() {
            current_value = 0.0;
        }

        [[nodiscard]] bool is_empty() {
            return (input_connections.empty()) || (hidden_connections.empty());
        }

        void mutate_bias() {
            bias = change_delta(bias);
        }

        void mutate_neuron_type() {
            neuron_type = random_neuron_type();
        }

        [[nodiscard]] bool has_input_connection(size_t index) {
            for (auto [index2, weight2]: input_connections) {
                if (index == index2) {
                    return true;
                }
            }

            return false;
        }

        void add_input_connection(size_t index) {
            if (has_input_connection(index)) {
                mutate_input_connection();
                current_mut_op = 4;
            } else {
                std::float64_t weight = global_rng.get_float64_unif1();
                input_connections.push_back({index, weight});
                current_mut_op = 1;
            }
        }

        void mutate_input_connection() {
            size_t n = input_connections.size();

            if (n == 0) {
                mutate_bias();
                current_mut_op = 0;
            } else if (n == 1) {
                std::float64_t value = input_connections[0].second;
                input_connections[0].second = change_delta(value);
                current_mut_op = 4;
            } else {
                size_t index = global_rng.get_size_t(n);
                std::float64_t value = input_connections[index].second;
                input_connections[index].second = change_delta(value);
                current_mut_op = 4;
            }
        }

        [[nodiscard]] bool has_hidden_connection(size_t index) {
            for (auto [index2, weight2]: hidden_connections) {
                if (index == index2) {
                    return true;
                }
            }

            return false;
        }

        void add_hidden_connection(size_t index) {
            if (has_hidden_connection(index)) {
                mutate_hidden_connection();
                current_mut_op = 5;
            } else {
                std::float64_t weight = global_rng.get_float64_unif1();
                hidden_connections.push_back({index, weight});
                current_mut_op = 2;
            }
        }

        void mutate_hidden_connection() {
            size_t n = hidden_connections.size();

            if (n == 0) {
                mutate_bias();
                current_mut_op = 0;
            } else if (n == 1) {
                std::float64_t value = hidden_connections[0].second;
                hidden_connections[0].second = change_delta(value);
                current_mut_op = 5;
            } else {
                size_t index = global_rng.get_size_t(n);
                std::float64_t value = hidden_connections[index].second;
                hidden_connections[index].second = change_delta(value);
                current_mut_op = 5;
            }
        }

        void optimize() {
            const std::float64_t factor = 1.001;
            const std::float64_t limit1 = 0.6;
            const std::float64_t limit2 = 1.0 - limit1;
            bool mutation_needed = true;

            if ((bias > limit1) || (bias < -limit1)) {
                bias = bias * factor;
                mutation_needed = false;
            } else if ((bias > -limit2) && (bias < limit2)) {
                bias = bias / factor;
                mutation_needed = false;
            }

            for (auto &[index, weight]: input_connections) {
                if ((weight > limit1) || (weight < -limit1)) {
                    weight = weight * factor;
                    mutation_needed = false;
                } else if ((weight > -limit2) && (weight < limit2)) {
                    weight = weight / factor;
                    mutation_needed = false;
                }
            }

            for (auto &[index, weight]: hidden_connections) {
                if ((weight > limit1) || (weight < -limit1)) {
                    weight = weight * factor;
                    mutation_needed = false;
                } else if ((weight > -limit2) && (weight < limit2)) {
                    weight = weight / factor;
                    mutation_needed = false;
                }
            }

            if (mutation_needed) {
                mutate_hidden_connection();
                current_mut_op = 5;
            } else {
                current_mut_op = 6;
            }
        }

        void evaluate(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            switch (neuron_type) {
                case NeuronType::WeightSum:
                    evaluate_weight_sum(input_values, hidden_layer);
                    break;
                case NeuronType::WeightProduct:
                    evaluate_weight_product(input_values, hidden_layer);
                    break;
                case NeuronType::Min:
                    evaluate_min(input_values, hidden_layer);
                    break;
                case NeuronType::Max:
                    evaluate_max(input_values, hidden_layer);
                    break;
                default:
                    throw std::runtime_error(fmt::format("Unknown neuron type operation: {}", std::to_underlying(neuron_type)));
                    break;
            }

            // ReLU:
            if (current_value < 0.0) {
                current_value = 0.0;
            }
        }

        void evaluate_weight_sum(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            current_value = bias;

            for (auto [index, weight]: input_connections) {
                current_value += weight * input_values[index];
            }

            for (auto [index, weight]: hidden_connections) {
                current_value += weight * hidden_layer[index].current_value;
            }
        }

        void evaluate_weight_product(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            current_value = bias;

            for (auto [index, weight]: input_connections) {
                current_value *= weight * input_values[index];
            }

            for (auto [index, weight]: hidden_connections) {
                current_value *= weight * hidden_layer[index].current_value;
            }
        }

        void evaluate_min(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            current_value = bias;

            if (input_connections.size() > 0) {
                std::float64_t chosen_value = input_values[input_connections[0].first];
                std::float64_t chosen_weight = 0.0;

                for (auto [index, weight]: input_connections) {
                    if (input_values[index] < chosen_value) {
                        chosen_value = input_values[index];
                        chosen_weight = weight;
                    }
                }

                current_value += chosen_weight * chosen_value;
            }


            if (hidden_connections.size() > 0) {
                std::float64_t chosen_value = hidden_layer[hidden_connections[0].first].current_value;
                std::float64_t chosen_weight = 0.0;

                for (auto [index, weight]: hidden_connections) {
                    if (hidden_layer[index].current_value < chosen_value) {
                        chosen_value = hidden_layer[index].current_value;
                        chosen_weight = weight;
                    }
                }

                current_value += chosen_weight * chosen_value;
            }
        }

        void evaluate_max(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            current_value = bias;

            if (input_connections.size() > 0) {
                std::float64_t chosen_value = input_values[input_connections[0].first];
                std::float64_t chosen_weight = 0.0;

                for (auto [index, weight]: input_connections) {
                    if (input_values[index] > chosen_value) {
                        chosen_value = input_values[index];
                        chosen_weight = weight;
                    }
                }

                current_value += chosen_weight * chosen_value;
            }


            if (hidden_connections.size() > 0) {
                std::float64_t chosen_value = hidden_layer[hidden_connections[0].first].current_value;
                std::float64_t chosen_weight = 0.0;

                for (auto [index, weight]: hidden_connections) {
                    if (hidden_layer[index].current_value > chosen_value) {
                        chosen_value = hidden_layer[index].current_value;
                        chosen_weight = weight;
                    }
                }

                current_value += chosen_weight * chosen_value;
            }
        }

        [[nodiscard]] nlohmann::json to_json() {
            nlohmann::json json_input_con = nlohmann::json::array();
            nlohmann::json json_hidden_con = nlohmann::json::array();

            for (auto con: input_connections) {
                json_input_con.push_back({con.first, double(con.second)});
            }

            for (auto con: hidden_connections) {
                json_hidden_con.push_back({con.first, double(con.second)});
            }

            const nlohmann::json result = {
                {"bias", double(bias)},
                {"input_connections", json_input_con},
                {"hidden_connections", json_hidden_con},
                {"neuron_type", neuron_type}
            };

            return result;
        }

        void from_json(nlohmann::json neuron) {
            bias = neuron["bias"].get<double>();
            input_connections.clear();
            hidden_connections.clear();

            for (auto p: neuron["input_connections"]) {
                input_connections.push_back({p[0].get<size_t>(), p[1].get<double>()});
            }

            for (auto p: neuron["hidden_connections"]) {
                hidden_connections.push_back({p[0].get<size_t>(), p[1].get<double>()});
            }

            neuron_type = static_cast<NeuronType>(neuron["neuron_type"].get<uint8_t>());
        }

        [[nodiscard]] uint32_t total_connections() {
            return static_cast<uint32_t>(input_connections.size() + hidden_connections.size());
        }
};
