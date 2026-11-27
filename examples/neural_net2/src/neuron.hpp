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

//SE_RNG_L64 global_rng;
SE_RNG_WY global_rng;

const std::float64_t opt_factor = 1.001;
const std::float64_t opt_limit1 = 0.6;
const std::float64_t opt_limit2 = 1.0 - opt_limit1;
const std::float64_t value_limit = 10.0;

using Connection = std::pair<size_t, std::float64_t>;

[[nodiscard]] std::float64_t clamp_value(std::float64_t value) {
    return std::clamp<std::float64_t>(value, -value_limit, value_limit);
}

[[nodiscard]] std::float64_t change_delta(std::float64_t value) {
    const uint8_t n = global_rng.get_uint8(10);

    if (n == 0) {
        return global_rng.get_float64_unif1();
    } else {
        const std::float64_t delta = global_rng.get_float64_unif2();

        value += delta;
        return clamp_value(value);
    }
}

[[nodiscard]] bool outside_limit(std::float64_t value) {
    return ((value < -opt_limit1) || (value > opt_limit1));
}

[[nodiscard]] bool inside_limit(std::float64_t value) {
    return ((value > -opt_limit2) && (value < opt_limit2));
}

class Neuron {
    private:
        std::vector<Connection> input_connections;
        std::vector<Connection> hidden_connections;
        std::float64_t bias;

    public:
        uint8_t current_mut_op;
        std::float64_t current_value;

        Neuron():
            input_connections(),
            hidden_connections(),
            bias(global_rng.get_float64_unif1()),
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
            current_mut_op = 0;
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
            current_mut_op = 1;

            if (has_input_connection(index)) {
                mutate_input_connection();
            } else {
                std::float64_t weight = global_rng.get_float64_unif1();
                input_connections.push_back({index, weight});
            }
        }

        void mutate_input_connection() {
            current_mut_op = 4;
            const size_t n = input_connections.size();

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
            current_mut_op = 2;

            if (has_hidden_connection(index)) {
                mutate_hidden_connection();
            } else {
                std::float64_t weight = global_rng.get_float64_unif1();
                hidden_connections.push_back({index, weight});
            }
        }

        void mutate_hidden_connection() {
            current_mut_op = 5;
            const size_t n = hidden_connections.size();

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

        void optimize_bias() {
            current_mut_op = 6;

            if (outside_limit(bias)) {
                bias = clamp_value(bias * opt_factor);
            } else if (inside_limit(bias)) {
                bias /= opt_factor;
            } else {
                mutate_bias();
            }
        }

        void optimize_input_connection() {
            current_mut_op = 7;

            const size_t n = input_connections.size();

            if (n == 0) {
                mutate_bias();
            } else {
                size_t index = 0;

                if (n > 1) {
                    index = global_rng.get_size_t(n);
                }

                std::float64_t &value = input_connections[index].second;

                if (outside_limit(value)) {
                    value = clamp_value(value * opt_factor);
                } else if (inside_limit(value)) {
                    value /= opt_factor;
                } else {
                    mutate_input_connection();
                }
            }
        }

        void optimize_hidden_connection() {
            current_mut_op = 8;

            const size_t n = hidden_connections.size();

            if (n == 0) {
                mutate_bias();
            } else {
                size_t index = 0;

                if (n > 1) {
                    index = global_rng.get_size_t(n);
                }

                std::float64_t &value = hidden_connections[index].second;

                if (outside_limit(value)) {
                    value = clamp_value(value * opt_factor);
                } else if (inside_limit(value)) {
                    value /= opt_factor;
                } else {
                    mutate_hidden_connection();
                }
            }
        }

        void optimize() {
            current_mut_op = 9;
            bool other_mutation_needed = true;

            if (outside_limit(bias)) {
                bias = clamp_value(bias * opt_factor);
                other_mutation_needed = false;
            } else if (inside_limit(bias)) {
                bias /= opt_factor;
                other_mutation_needed = false;
            }

            for (auto &[index, weight]: input_connections) {
                if (outside_limit(weight)) {
                    weight = clamp_value(weight * opt_factor);
                    other_mutation_needed = false;
                } else if (inside_limit(weight)) {
                    weight /= opt_factor;
                    other_mutation_needed = false;
                }
            }

            for (auto &[index, weight]: hidden_connections) {
                if (outside_limit(weight)) {
                    weight = clamp_value(weight * opt_factor);
                    other_mutation_needed = false;
                } else if (inside_limit(weight)) {
                    weight /= opt_factor;
                    other_mutation_needed = false;
                }
            }

            if (other_mutation_needed) {
                mutate_hidden_connection();
            }
        }

        void multiply_bias(std::float64_t factor) {
            bias = clamp_value(bias * factor);
        }

        void multiply_input_connection(std::float64_t factor, uint8_t mut_op) {
            current_mut_op = mut_op;
            const size_t n = input_connections.size();

            if (n == 0) {
                mutate_bias();
            } else {
                size_t index = 0;

                if (n > 1) {
                    index = global_rng.get_size_t(n);
                }

                std::float64_t &value = input_connections[index].second;
                value = clamp_value(value * factor);
            }
        }

        void multiply_hidden_connection(std::float64_t factor, uint8_t mut_op) {
            current_mut_op = mut_op;
            const size_t n = hidden_connections.size();

            if (n == 0) {
                mutate_bias();
            } else {
                size_t index = 0;

                if (n > 1) {
                    index = global_rng.get_size_t(n);
                }

                std::float64_t &value = hidden_connections[index].second;
                value = clamp_value(value * factor);
            }
        }

        void set_bias_to(std::float64_t new_value) {
            bias = clamp_value(new_value);
        }

        void set_input_connection_to(std::float64_t new_value, uint8_t mut_op) {
            current_mut_op = mut_op;
            const size_t n = input_connections.size();

            if (n == 0) {
                mutate_bias();
            } else {
                size_t index = 0;

                if (n > 1) {
                    index = global_rng.get_size_t(n);
                }

                std::float64_t &value = input_connections[index].second;
                value = clamp_value(new_value);
            }
        }

        void set_hidden_connection_to(std::float64_t new_value, uint8_t mut_op) {
            current_mut_op = mut_op;
            const size_t n = hidden_connections.size();

            if (n == 0) {
                mutate_bias();
            } else {
                size_t index = 0;

                if (n > 1) {
                    index = global_rng.get_size_t(n);
                }

                std::float64_t &value = hidden_connections[index].second;
                value = clamp_value(new_value);
            }
        }

        void evaluate(std::span<const std::float64_t> input_values, std::span<const Neuron> hidden_layer) {
            current_value = bias;

            for (auto [index, weight]: input_connections) {
                current_value += weight * input_values[index];
            }

            for (auto [index, weight]: hidden_connections) {
                current_value += weight * hidden_layer[index].current_value;
            }

            // ReLU:
            if (current_value < 0.0) {
                current_value = 0.0;
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
