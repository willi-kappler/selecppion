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

class Neuron {
    private:
        std::vector<Connection> input_connections;
        std::vector<Connection> hidden_connections;
        std::float64_t bias;
        uint8_t activation_method;

    public:
        std::float64_t current_value;

        Neuron():
            input_connections(),
            hidden_connections(),
            bias(global_rng.get_float64_unif1()),
            activation_method(0),
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

        void mutate_activation() {
            activation_method = global_rng.get_uint8(9);
        }

        bool has_input_connection(size_t index) {
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
            } else {
                std::float64_t weight = global_rng.get_float64_unif1();
                input_connections.push_back({index, weight});
            }
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

        void mutate_all_input_connections() {
            std::float64_t value;

            for (size_t i = 0; i < input_connections.size(); i++) {
                value = input_connections[i].second;
                input_connections[i].second = change_delta(value);
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
                if (has_input_connection(new_index)) {
                    mutate_input_connection();
                } else {
                    input_connections[index].first = new_index;
                }
            }
        }

        bool has_hidden_connection(size_t index) {
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
            } else {
                std::float64_t weight = global_rng.get_float64_unif1();
                hidden_connections.push_back({index, weight});
            }
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

        void mutate_all_hidden_connections() {
            std::float64_t value;

            for (size_t i = 0; i < hidden_connections.size(); i++) {
                value = hidden_connections[i].second;
                hidden_connections[i].second = change_delta(value);
            }
        }

        void replace_hidden_connection(size_t new_index) {
            size_t n = hidden_connections.size();

            if (n == 0) {
                mutate_bias();
            } else if (n == 1) {
                hidden_connections[0].first = new_index;
            } else {
                if (has_hidden_connection(new_index)) {
                    mutate_hidden_connection();
                } else {
                    size_t index = global_rng.get_size_t(n);
                    hidden_connections[index].first = new_index;
                }
            }
        }

        Neuron split_neuron() {
            Neuron new_neuron;

            new_neuron.bias = bias;
            new_neuron.activation_method = activation_method;

            const size_t input_size = input_connections.size();
            const size_t input_size_2 = input_size / 2;
            const size_t hidden_size = hidden_connections.size();
            const size_t hidden_size_2 = hidden_size / 2;

            if ((input_size > 1) && (hidden_size > 1)) {
                // Move half of the input connections into the new neuron:
                new_neuron.input_connections.assign(input_connections.begin() + input_size_2, input_connections.end());
                input_connections.erase(input_connections.begin() + input_size_2, input_connections.end());

                // Move half of the hidden connections into the new neuron:
                new_neuron.hidden_connections.assign(hidden_connections.begin() + hidden_size_2, hidden_connections.end());
                hidden_connections.erase(hidden_connections.begin() + hidden_size_2, hidden_connections.end());
            }

            return new_neuron;
        }

        void evaluate(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            std::float64_t new_value = bias;

            for (auto [index, weight]: input_connections) {
                new_value += weight * input_values[index];
            }

            for (auto [index, weight]: hidden_connections) {
                new_value += weight * hidden_layer[index].current_value;
            }

            switch (activation_method) {
                case 0:
                    // ReLU:
                    if (new_value < 0.0) {
                        new_value = 0.0;
                    }
                    break;
                case 1:
                    // Leaky ReLU:
                    if (new_value < 0.0) {
                        new_value = 0.01 * new_value;
                    }
                    break;
                case 2:
                    // Scale by connections:
                    if (new_value <= 0.0) {
                        new_value = 0.0;
                    } else {
                        new_value = new_value / static_cast<std::float64_t>(input_connections.size() + hidden_connections.size() + 1);
                    }
                    break;
                case 3:
                    // Min-Max limit 1:
                    if (new_value <= -10.0) {
                        new_value = -10.0;
                    } else if (new_value >= 10.0) {
                        new_value = 10.0;
                    }
                    break;
                case 4:
                    // Min-Max limit 2:
                    if (new_value < 0.0) {
                        new_value = 0.0;
                    } else if (new_value >= 10.0) {
                        new_value = 10.0;
                    }
                    break;
                case 5:
                    // Sigmoid (standard logistic function):
                    new_value = std::clamp<std::float64_t>(new_value, -100.0, 100.0);
                    new_value = 1.0 / (1.0 + std::exp(-new_value));
                    break;
                case 6:
                    // Hyperbolic tangent:
                    {
                        new_value = std::clamp<std::float64_t>(new_value, -100.0, 100.0);
                        std::float64_t e1 = std::exp(new_value);
                        std::float64_t e2 = std::exp(-new_value);
                        new_value = (e1 - e2) / (e1 + e2);
                    }
                    break;
                case 7:
                    // Exponential linear unit (ELU):
                    if (new_value < 0.0) {
                        new_value = std::clamp<std::float64_t>(new_value, -100.0, 100.0);
                        new_value = std::exp(new_value) - 1.0;
                    }
                    break;
                case 8:
                    // Algebraic function:
                    // new_value = std::abs(new_value);
                    new_value = new_value * new_value;
                    new_value = new_value / (1.0 + new_value);
                    break;
            }

            current_value = new_value;
        }

        void evaluate_log(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer, std::shared_ptr<spdlog::logger> se_logger) {
            std::float64_t new_value = bias;

            se_logger->debug("Neuron::evaluate_log()");

            for (auto [index, weight]: input_connections) {
                se_logger->debug("input_connection: index: {}, weight: {}, new_value: {}", index, weight, new_value);
                new_value += weight * input_values[index];
                se_logger->debug("input_value: {}", input_values[index]);
                se_logger->debug("new_value: {}", new_value);
            }

            for (auto [index, weight]: hidden_connections) {
                se_logger->debug("hidden_connection: index: {}, weight: {}, new_value: {}", index, weight, new_value);
                new_value += weight * hidden_layer[index].current_value;
                se_logger->debug("hidden_layer value: {}", hidden_layer[index].current_value);
                se_logger->debug("new_value: {}", new_value);
            }

            if (new_value < 0.0) {
                new_value = 0.0;
            }

            se_logger->debug("activation: new_value: {}", new_value);

            current_value = new_value;
            se_logger->flush();
        }

        [[nodiscard]] Neuron clone() {
            Neuron result;

            result.input_connections = input_connections;
            result.hidden_connections = hidden_connections;
            result.bias = bias;
            result.activation_method = activation_method;

            return result;
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
                {"activation", activation_method},
                {"input_connections", json_input_con},
                {"hidden_connections", json_hidden_con}
            };

            return result;
        }

        void from_json(nlohmann::json neuron) {
            bias = neuron["bias"].get<double>();
            activation_method = neuron["activation"].get<uint8_t>();
            input_connections.clear();
            hidden_connections.clear();

            for (auto p: neuron["input_connections"]) {
                input_connections.push_back({p[0].get<size_t>(), p[1].get<double>()});
            }

            for (auto p: neuron["hidden_connections"]) {
                hidden_connections.push_back({p[0].get<size_t>(), p[1].get<double>()});
            }
        }

        [[nodiscard]] uint32_t total_connections() {
            return static_cast<uint32_t>(input_connections.size() + hidden_connections.size());
        }

        void print_neuron() {
            std::print("Bias: {}, value: {}\n", bias, current_value);
            std::print("Activation method: {}\n", activation_method);
            std::print("Input: {}\n", input_connections);
            std::print("Hidden: {}\n", hidden_connections);
        }
};
