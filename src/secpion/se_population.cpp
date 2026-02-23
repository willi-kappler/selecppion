/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for one population
*/

// External include:
#include <spdlog/sinks/basic_file_sink.h>

// Local includes:
#include "se_population.hpp"

namespace secpion {
SEPopulation::SEPopulation(SEConfiguration config):
    se_config(config),
    se_logger(),
    population(),
    best_index(0),
    worst_index(0),
    mut_op_index(0),
    minimum_found(false),
    rng()
{
    rng.seed();
}

void SEPopulation::se_set_logger(std::shared_ptr<spdlog::logger> logger) {
    se_logger = logger;

    se_logger->debug("Population size: {}, target fitness 1: {}, target fitness 2: {}",
        se_config.node_population_size, se_config.target_fitness1, se_config.target_fitness2);
    se_logger->debug("Number of iterations: {}, number of mutations: {}",
        se_config.num_of_iterations, se_config.num_of_mutations);
    se_logger->debug("Randomize population: {}, randomize count: {}",
        se_config.randomize_population, se_config.randomize_count);
    se_logger->debug("Accept new best: {}", se_config.accept_new_best);

    //se_logger->debug("Accept new best: {}, mutation operations: {}",
    //    config.accept_new_best, config.mutation_operations);
}

void SEPopulation::se_set_loglevel(spdlog::level::level_enum level) {
    se_logger->set_level(level);
}

void SEPopulation::se_set_file_logger(std::string_view prefix = "selecppion") {
    std::string file_name = nodcru2::nc_gen_log_file_name(prefix);
    std::shared_ptr<spdlog::logger> file_logger = spdlog::basic_logger_mt("se_logger", file_name);
    se_set_logger(file_logger);
}

}
