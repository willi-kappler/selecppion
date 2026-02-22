/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for one population
*/

// Local includes:
#include "se_population.hpp"

namespace secpion {
SEPopulation::SEPopulation(SEConfiguration config):
    config_intern(config),
    se_logger(),
    population(),
    best_index(0),
    worst_index(0),
    mut_op_index(0),
    minimum_found(false),
    rng()
{
    rng.seed();
    se_logger->debug("Population size: {}, target fitness 1: {}, target fitness 2: {}",
        config.node_population_size, config.target_fitness1, config.target_fitness2);
    se_logger->debug("Number of iterations: {}, number of mutations: {}",
        config.num_of_iterations, config.num_of_mutations);
    se_logger->debug("Randomize population: {}, randomize count: {}",
        config.randomize_population, config.randomize_count);
    //se_logger->debug("Accept new best: {}, mutation operations: {}",
    //    config.accept_new_best, config.mutation_operations);


    // spdlog::get("mandel_logger")->debug("Initial data received: {}", mandel_data);

}


}
