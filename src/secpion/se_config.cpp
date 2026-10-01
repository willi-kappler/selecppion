/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the configuration options
*/

// STD includes:
#include <fstream>
#include <iostream>

// Local includes:
#include "se_config.hpp"
#include "se_exceptions.hpp"

namespace secpion {
SEConfiguration::SEConfiguration():
    // Server configuration:
    target_fitness1(0.0),
    target_fitness2(0.0),
    result_filename("best_result.json"),
    save_new_fitness(true),
    allow_same_fitness(false),
    share_only_best(true),
    server_population_size(10),
    se_server_log_file(""),
    se_server_log_level(""),

    // Node configuration:
    node_population_size(10),
    num_of_iterations(1000),
    num_of_mutations(10),
    accept_new_best(true),
    randomize_population(false),
    randomize_count(5),
    population_kind(1),
    mutation_operations(),
    early_exit_sleep(10),
    se_node_log_file(""),
    se_node_log_level(""),
    seed_count(10),

    min_num_of_individuals(2),
    sine_base(100.0),
    sine_amplitude(50.0),
    sine_frequency(0.01),
    limit_factor(2.0),
    mutation_probability(0.1),
    crossover_probability(0.9)
{}

[[nodiscard]] SEConfiguration se_config_from_json(const nlohmann::json json_config) {
    SEConfiguration se_config;

    if (json_config.contains("target_fitness1")) {
        se_config.target_fitness1 = json_config["target_fitness1"].get<double>(); // doesn't like std::float64_t
    }

    if (json_config.contains("target_fitness2")) {
        se_config.target_fitness2 = json_config["target_fitness2"].get<double>();
    }

    if (json_config.contains("result_filename")) {
        se_config.result_filename = json_config["result_filename"].get<std::string>();

        if (se_config.result_filename.size() == 0) {
            throw SEConfigurationException("result_filename is empty!");
        }
    }

    if (json_config.contains("save_new_fitness")) {
        se_config.save_new_fitness = json_config["save_new_fitness"].get<bool>();
    }

    if (json_config.contains("allow_same_fitness")) {
        se_config.allow_same_fitness = json_config["allow_same_fitness"].get<bool>();
    }

    if (json_config.contains("share_only_best")) {
        se_config.share_only_best = json_config["share_only_best"].get<bool>();
    }

    if (json_config.contains("server_population_size")) {
        se_config.server_population_size = json_config["server_population_size"].get<uint32_t>();

        if (se_config.server_population_size < 2) {
            throw SEConfigurationException("server_population_size < 2!");
        }
    }

    if (json_config.contains("se_server_log_file")) {
        se_config.se_server_log_file = json_config["se_server_log_file"].get<std::string>();
    }

    if (json_config.contains("se_server_log_level")) {
        se_config.se_server_log_level = json_config["se_server_log_level"].get<std::string>();
    }

    // Node settings:
    if (json_config.contains("node_population_size")) {
        se_config.node_population_size = json_config["node_population_size"].get<uint32_t>();

        if (se_config.node_population_size < 2) {
            throw SEConfigurationException("node_population_size < 2!");
        }
    }

    if (json_config.contains("num_of_iterations")) {
        se_config.num_of_iterations = json_config["num_of_iterations"].get<uint32_t>();

        if (se_config.num_of_iterations < 2) {
            throw SEConfigurationException("num_of_iterations < 2!");
        }
    }

    if (json_config.contains("num_of_mutations")) {
        se_config.num_of_mutations = json_config["num_of_mutations"].get<uint32_t>();

        if (se_config.num_of_mutations == 0) {
            throw SEConfigurationException("num_of_mutations == 0!");
        }
    }

    if (json_config.contains("random_num_of_mutations")) {
        se_config.random_num_of_mutations = json_config["random_num_of_mutations"].get<bool>();
    }

    if (json_config.contains("accept_new_best")) {
        se_config.accept_new_best = json_config["accept_new_best"].get<bool>();
    }

    if (json_config.contains("randomize_population")) {
        se_config.randomize_population = json_config["randomize_population"].get<bool>();
    }

    if (json_config.contains("randomize_count")) {
        se_config.randomize_count = json_config["randomize_count"].get<uint32_t>();
    }

    if (json_config.contains("population_kind")) {
        se_config.population_kind = json_config["population_kind"].get<uint8_t>();

        if ((se_config.population_kind < 1) || (se_config.population_kind > 8)) {
            throw SEConfigurationException("population_kind must be between 1 and 8!");
        }
    }

    if (json_config.contains("mutation_operations")) {
        se_config.mutation_operations = std::vector<uint8_t>();

        for (auto &data: json_config["mutation_operations"]) {
            se_config.mutation_operations.push_back(data.get<uint8_t>());
        }
    }

    if (json_config.contains("early_exit_sleep")) {
        se_config.early_exit_sleep = json_config["early_exit_sleep"].get<uint8_t>();
    }

    if (json_config.contains("se_node_log_file")) {
        se_config.se_node_log_file = json_config["se_node_log_file"].get<std::string>();
    }

    if (json_config.contains("se_node_log_level")) {
        se_config.se_node_log_level = json_config["se_node_log_level"].get<std::string>();
    }

    if (json_config.contains("seed_count")) {
        se_config.seed_count = json_config["seed_count"].get<uint32_t>();
    }

    if (json_config.contains("min_num_of_individuals")) {
        se_config.min_num_of_individuals = json_config["min_num_of_individuals"].get<uint8_t>();
    }

    if (json_config.contains("sine_base")) {
        se_config.sine_base = json_config["sine_base"].get<double>();
    }

    if (json_config.contains("sine_amplitude")) {
        se_config.sine_amplitude = json_config["sine_amplitude"].get<double>();

        if (se_config.sine_amplitude <= 0.0) {
            throw SEConfigurationException("sine_amplitude must be > 0!");
        }
    }

    if (json_config.contains("sine_frequency")) {
        se_config.sine_frequency = json_config["sine_frequency"].get<double>();

        if (se_config.sine_frequency <= 0.0) {
            throw SEConfigurationException("sine_frequency must be > 0!");
        }
    }

    if (json_config.contains("limit_factor")) {
        se_config.limit_factor = json_config["limit_factor"].get<double>();
    }

    if (json_config.contains("mutation_probability")) {
        se_config.mutation_probability = json_config["mutation_probability"].get<double>();
    }

    if (json_config.contains("crossover_probability")) {
        se_config.crossover_probability = json_config["crossover_probability"].get<double>();
    }

    return se_config;
}

[[nodiscard]] std::string se_file_to_string(std::filesystem::path file_path) {
    std::ifstream in_file(file_path);

    if (in_file.is_open()) {
        std::string file_contents {std::istreambuf_iterator<char>(in_file), std::istreambuf_iterator<char>()};
        return file_contents;
    } else {
        throw SEConfigurationException("Open file error");
    }
}

[[nodiscard]] SEConfiguration se_config_from_string(std::string_view config_as_string) {
    const nlohmann::json json_config = nlohmann::json::parse(config_as_string);

    return se_config_from_json(json_config);
}

[[nodiscard]] SEConfiguration se_config_from_file(std::filesystem::path file_path) {
    return se_config_from_string(se_file_to_string(file_path));
}
}
