/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the configuration options
*/

#ifndef FILE_SE_CONFIG_HPP_INCLUDED
#define FILE_SE_CONFIG_HPP_INCLUDED

// STD includes:
#include <string>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <string_view>
#include <stdfloat>

// External includes:
// #include <nc_config.hpp>

namespace Selecppion {
class SEConfiguration {
    public:
        // Server configuration:
        // Maybe add NodeCrunch2 NCConfiguration here
        std::float64_t target_fitness1;
        std::float64_t target_fitness2;
        std::string result_filename;
        bool save_new_fitness;
        bool allow_same_fitness;
        bool share_only_best;
        uint32_t server_population_size;

        // Node configuration:
        uint32_t node_population_size;
        uint32_t num_of_iterations;
        uint32_t num_of_mutations;
        bool accept_new_best;
        bool randomize_population;
        uint32_t randomize_count;
        uint8_t population_kind;
        std::vector<uint8_t> mutation_operations;
        
        // Population specific configuration:
        uint8_t min_num_of_individuals;
        std::float64_t sine_base;
        std::float64_t sine_amplitude;
        std::float64_t sine_frequency;
        std::float64_t limit_range;

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

[[nodiscard]] SEConfiguration se_config_from_string(std::string_view);

[[nodiscard]] SEConfiguration se_config_from_file(std::filesystem::path);

}

#endif // FILE_SE_CONFIG_HPP_INCLUDED