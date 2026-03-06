/*
    Selecppion
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
SEConfiguration::SEConfiguration(NCConfiguration nc_config):
    // Server configuration:
    nc_config(nc_config),
    target_fitness1(0.0),
    target_fitness2(0.0),
    result_filename("best_result.json"),
    save_new_fitness(true),
    allow_same_fitness(false),
    share_only_best(true),
    server_population_size(10),

    // Node configuration:
    node_population_size(10),
    num_of_iterations(1000),
    num_of_mutations(10),
    accept_new_best(true),
    randomize_population(false),
    randomize_count(10),
    population_kind(1),
    mutation_operations(),

    min_num_of_individuals(2),
    sine_base(100.0),
    sine_amplitude(50.0),
    sine_frequency(10.0),
    limit_range(10.0)
{}

[[nodiscard]] SEConfiguration se_config_from_json(const tao::json::value json_config) {
    NCConfiguration nc_config = nc_config_from_json(json_config);
    SEConfiguration se_config(nc_config);

    if (auto v = json_config.find("target_fitness1"); v != nullptr) {
        se_config.target_fitness1 = v->as<double>(); // doesn't like std::float64_t
    }

    if (auto v = json_config.find("target_fitness2"); v != nullptr) {
        se_config.target_fitness2 = v->as<double>();
    }

    if (auto v = json_config.find("result_filename"); v != nullptr) {
        se_config.result_filename = v->as<std::string>();

        if (se_config.result_filename.size() == 0) {
            throw SEConfigurationException("result_filename is empty!");
        }
    }

    if (auto v = json_config.find("save_new_fitness"); v != nullptr) {
        se_config.save_new_fitness = v->as<bool>();
    }

    if (auto v = json_config.find("allow_same_fitness"); v != nullptr) {
        se_config.allow_same_fitness = v->as<bool>();
    }

    if (auto v = json_config.find("share_only_best"); v != nullptr) {
        se_config.share_only_best = v->as<bool>();
    }

    if (auto v = json_config.find("server_population_size"); v != nullptr) {
        se_config.server_population_size = v->as<uint32_t>();

        if (se_config.server_population_size < 2) {
            throw SEConfigurationException("server_population_size < 2!");
        }
    }

    if (auto v = json_config.find("node_population_size"); v != nullptr) {
        se_config.node_population_size = v->as<uint32_t>();

        if (se_config.node_population_size < 2) {
            throw SEConfigurationException("node_population_size < 2!");
        }
    }

    if (auto v = json_config.find("num_of_iterations"); v != nullptr) {
        se_config.num_of_iterations = v->as<uint32_t>();

        if (se_config.num_of_iterations < 2) {
            throw SEConfigurationException("num_of_iterations < 2!");
        }
    }

    if (auto v = json_config.find("num_of_mutations"); v != nullptr) {
        se_config.num_of_mutations = v->as<uint32_t>();

        if (se_config.num_of_mutations == 0) {
            throw SEConfigurationException("num_of_mutations == 0!");
        }
    }

    if (auto v = json_config.find("random_num_of_mutations"); v != nullptr) {
        se_config.random_num_of_mutations = v->as<bool>();
    }

    if (auto v = json_config.find("accept_new_best"); v != nullptr) {
        se_config.accept_new_best = v->as<bool>();
    }

    if (auto v = json_config.find("randomize_population"); v != nullptr) {
        se_config.randomize_population = v->as<bool>();
    }

    if (auto v = json_config.find("randomize_count"); v != nullptr) {
        se_config.randomize_count = v->as<uint32_t>();
    }

    if (auto v = json_config.find("population_kind"); v != nullptr) {
        se_config.population_kind = v->as<uint8_t>();

        if ((se_config.population_kind < 1) || (se_config.population_kind > 8)) {
            throw SEConfigurationException("population_kind must be between 1 and 8!");
        }
    }

    if (auto v = json_config.find("mutation_operations"); v != nullptr) {
        se_config.mutation_operations = std::vector<uint8_t>();

        for (auto data: v->get_array()) {
            se_config.mutation_operations.push_back(data.as<uint8_t>());
        }
    }

    if (auto v = json_config.find("min_num_of_individuals"); v != nullptr) {
        se_config.min_num_of_individuals = v->as<uint8_t>();
    }

    if (auto v = json_config.find("sine_base"); v != nullptr) {
        se_config.sine_base = v->as<double>();
    }

    if (auto v = json_config.find("sine_amplitude"); v != nullptr) {
        se_config.sine_amplitude = v->as<double>();

        if (se_config.sine_amplitude <= 0.0) {
            throw SEConfigurationException("sine_amplitude must be > 0!");
        }
    }

    if (auto v = json_config.find("sine_frequency"); v != nullptr) {
        se_config.sine_frequency = v->as<double>();

        if (se_config.sine_frequency <= 0.0) {
            throw SEConfigurationException("sine_frequency must be > 0!");
        }
    }

    if (auto v = json_config.find("limit_range"); v != nullptr) {
        se_config.limit_range = v->as<double>();
    }

    return se_config;
}

[[nodiscard]] SEConfiguration se_config_from_string(std::string_view config_as_string) {
    const tao::json::value json_config = tao::json::from_string(config_as_string);

    return se_config_from_json(json_config);
}

[[nodiscard]] SEConfiguration se_config_from_file(std::filesystem::path file_path) {
    std::ifstream in_file {file_path};

    if (in_file.is_open()) {
        std::string file_contents {std::istreambuf_iterator<char>(in_file), std::istreambuf_iterator<char>()};
        return se_config_from_string(file_contents);
    } else {
        throw SEConfigurationException("Open file error");
    }
}
}