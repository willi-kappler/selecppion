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

        [[nodiscard]] bool is_empty() {
            return (input_connections.empty()) || (hidden_connections.empty());
        }

        void mutate_bias() {
            bias = change_delta(bias);
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

        void evaluate(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            std::float64_t new_value = bias;

            for (auto [index, weight]: input_connections) {
                new_value += weight * input_values[index];
            }

            for (auto [index, weight]: hidden_connections) {
                new_value += weight * hidden_layer[index].current_value;
            }

            // ReLU:
            if (new_value < 0.0) {
                new_value = 0.0;
            }

            current_value = new_value;
        }
/*
        [[nodiscard]] Neuron clone() {
            Neuron result;

            result.input_connections = input_connections;
            result.hidden_connections = hidden_connections;
            result.bias = bias;
            result.activation_method = activation_method;

            return result;
        }
*/
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
                {"hidden_connections", json_hidden_con}
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
        }

        [[nodiscard]] uint32_t total_connections() {
            return static_cast<uint32_t>(input_connections.size() + hidden_connections.size());
        }
};
