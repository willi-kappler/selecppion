/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the configuration options
*/

#ifndef FILE_SE_CONFIG_HPP_INCLUDED
#define FILE_SE_CONFIG_HPP_INCLUDED

// STD includes:
#include <string>
#include <cstdint>
#include <filesystem>
#include <string_view>
#include <stdfloat>

// External includes:
#include <tao/json.hpp>

namespace secpion {
class SEConfiguration {
    public:
        // Server configuration:
        std::float64_t target_fitness1;
        std::float64_t target_fitness2;
        std::string result_filename;
        bool save_new_fitness;
        bool allow_same_fitness;
        bool share_only_best;
        uint32_t server_population_size;
        std::string se_server_log_file;
        std::string se_server_log_level;

        // Node configuration:
        uint32_t node_population_size;
        uint32_t num_of_iterations;
        uint32_t num_of_mutations;
        bool random_num_of_mutations;
        bool accept_new_best;
        bool randomize_population;
        uint32_t randomize_count;
        uint8_t population_kind;
        std::vector<uint8_t> mutation_operations;
        uint8_t early_exit_sleep;
        std::string se_node_log_file;
        std::string se_node_log_level;
        uint32_t seed_count;

        // Population specific configuration:
        uint8_t min_num_of_individuals;
        std::float64_t sine_base;
        std::float64_t sine_amplitude;
        std::float64_t sine_frequency;
        std::float64_t limit_factor;
        std::float64_t mutation_probability;
        std::float64_t crossover_probability;

        // Constructor:
        SEConfiguration();

        // Default special member functions:
        SEConfiguration(SEConfiguration&&) = default;
        SEConfiguration(const SEConfiguration&) = default;

        // Disable all other special member functions:
        // SEConfiguration() = delete;
        SEConfiguration& operator=(const SEConfiguration&) = delete;
        SEConfiguration& operator=(const SEConfiguration&&) = delete;
};

[[nodiscard]] SEConfiguration se_config_from_json(const tao::json::value);

[[nodiscard]] SEConfiguration se_config_from_string(std::string_view);

[[nodiscard]] std::string se_file_to_string(std::filesystem::path);

[[nodiscard]] SEConfiguration se_config_from_file(std::filesystem::path);

}

#endif // FILE_SE_CONFIG_HPP_INCLUDED
