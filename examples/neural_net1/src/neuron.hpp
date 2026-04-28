/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the individual class for the neuralnet1 example

    To just build use:
    xmake build se_example_neuralnet1

    Run with:
    ./run_example.sh
*/

// STD includes:
#include <algorithm>
#include <vector>
#include <utility>
#include <span>
#include <print>

// Local includes:
#include "secpion/se_random.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

using Connection = std::pair<size_t, std::float64_t>;

[[nodiscard]] std::float64_t change_delta(std::float64_t value) {
    const uint8_t n = global_rng.get_uint8(100);

    if (n == 0) {
        return global_rng.get_float64_unif1();
    } else {
        const std::float64_t delta = global_rng.get_float64_unif2();

        value += delta;
        return std::clamp<std::float64_t>(value, -1.0, 1.0);
    }
}

class Neuron {
    private:
        std::vector<Connection> input_connections;
        std::vector<Connection> hidden_connections;
        std::float64_t bias;

    public:
        std::float64_t current_value;

        Neuron():
            input_connections(),
            hidden_connections(),
            bias(global_rng.get_float64_unif1()),
            current_value(0.0)
        {}

        void reset_value() {
            current_value = 0.0;
        }

        bool is_empty() {
            return (input_connections.empty()) && (hidden_connections.empty());
        }

        void mutate_bias() {
            bias = change_delta(bias);
        }

        void add_input_connection(size_t index) {
            for (auto [index2, weight2]: input_connections) {
                if (index == index2) {
                    mutate_input_connection();
                    return;
                }
            }

            std::float64_t weight = global_rng.get_float64_unif1();
            input_connections.push_back({index, weight});
        }

        void mutate_input_connection() {
            size_t n = input_connections.size();

            if (n == 0) {
                mutate_bias();
            } else if (n == 1) {
                std::float64_t value = input_connections[0].second;
                input_connections[0].second = change_delta(value);
            } else {
                size_t index = global_rng.get_size_t(n);
                std::float64_t value = input_connections[index].second;
                input_connections[index].second = change_delta(value);
            }
        }

        void replace_input_connection(size_t new_index) {
            size_t n = input_connections.size();

            if (n == 0) {
                mutate_bias();
            } else if (n == 1) {
                input_connections[0].first = new_index;
            } else {
                size_t index = global_rng.get_size_t(n);
                input_connections[index].first = new_index;
            }
        }

        void add_hidden_connection(size_t index) {
            for (auto [index2, weight2]: hidden_connections) {
                if (index == index2) {
                    mutate_hidden_connection();
                    return;
                }
            }

            std::float64_t weight = global_rng.get_float64_unif1();
            hidden_connections.push_back({index, weight});
        }

        void mutate_hidden_connection() {
            size_t n = hidden_connections.size();

            if (n == 0) {
                mutate_bias();
            } else if (n == 1) {
                std::float64_t value = hidden_connections[0].second;
                hidden_connections[0].second = change_delta(value);
            } else {
                size_t index = global_rng.get_size_t(n);
                std::float64_t value = hidden_connections[index].second;
                hidden_connections[index].second = change_delta(value);
            }
        }

        void replace_hidden_connection(size_t new_index) {
            size_t n = hidden_connections.size();

            if (n == 0) {
                mutate_bias();
            } else if (n == 1) {
                hidden_connections[0].first = new_index;
            } else {
                size_t index = global_rng.get_size_t(n);
                hidden_connections[index].first = new_index;
            }
        }

        void evaluate(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            std::float64_t new_value = bias;

            for (auto [index, weight]: input_connections) {
                new_value += weight * input_values[index];
            }

            for (auto [index, weight]: hidden_connections) {
                new_value += weight * hidden_layer[index].current_value;
            }

            // ReLU:
            current_value = std::max<std::float64_t>(0.0, new_value);
        }

        [[nodiscard]] Neuron clone() {
            Neuron result;

            result.input_connections = input_connections;
            result.hidden_connections = hidden_connections;
            result.bias = bias;

            return result;
        }

        [[nodiscard]] tao::json::value to_json() {
            tao::json::value json_input_con = tao::json::empty_array;
            tao::json::value json_hidden_con = tao::json::empty_array;
            tao::json::value json_pair = tao::json::empty_array;

            for (auto con: input_connections) {
                json_pair.get_array().clear();
                json_pair.get_array().push_back(con.first);
                json_pair.get_array().push_back(double(con.second));
                json_input_con.get_array().push_back(json_pair);
            }

            for (auto con: hidden_connections) {
                json_pair.get_array().clear();
                json_pair.get_array().push_back(con.first);
                json_pair.get_array().push_back(double(con.second));
                json_hidden_con.get_array().push_back(json_pair);
            }

            const tao::json::value result = {
                {"bias", double(bias)},
                {"input_connections", json_input_con},
                {"hidden_connections", json_hidden_con}
            };

            return result;
        }

        void from_json(tao::json::value neuron) {
            bias = neuron["bias"].as<double>();
            input_connections.clear();
            hidden_connections.clear();

            tao::json::value json_pair;
            for (auto p: neuron["input_connections"].get_array()) {
                json_pair = p.get_array();
                input_connections.push_back({json_pair[0].as<size_t>(), json_pair[1].as<double>()});
            }

            for (auto p: neuron["hidden_connections"].get_array()) {
                json_pair = p.get_array();
                hidden_connections.push_back({json_pair[0].as<size_t>(), json_pair[1].as<double>()});
            }

        }

        void print_neuron() {
            std::print("Bias: {}, value: {}\n", bias, current_value);
            std::print("Input: {}\n", input_connections);
            std::print("Hidden: {}\n", hidden_connections);
        }
};
