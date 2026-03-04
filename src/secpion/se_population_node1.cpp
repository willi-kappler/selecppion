/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 1.
*/

// Local includes:
#include "se_population_node1.hpp"

namespace secpion {
SEPopulationNode1::SEPopulationNode1(SEConfiguration se_config,
    std::unique_ptr<SEIndividual> individual):
    NCNodeDataProcessor(),
    population(se_config),
    offset(se_config.node_population_size / 2)
{
    population.se_fill_population(std::move(individual));
    population.se_sort_population();
}

[[nodiscard]] std::vector<uint8_t> SEPopulationNode1::nc_process_data([[maybe_unused]]std::vector<uint8_t> data) {
    std::vector<uint8_t> result;

    // population.se_randomize_or_accept_best();
    population.se_shuffle_mutation_operations();
    population.best_index = 0;
    population.worst_index = population.se_config.node_population_size - 1;

    std::unique_ptr<SEIndividual> cloned_indi;

    for (size_t i = 0; i < population.se_config.num_of_iterations; i++) {
        for (size_t j = 0; j < offset; j++) {
            // Create a copy of each individual before mutating it (lower half):
            cloned_indi = population.population[j]->se_clone();
            cloned_indi->se_randomize();
            cloned_indi->se_calculate_fitness1();
            // Overwrite upper half (the bad ones):
            population.population[j + offset] = std::move(cloned_indi);
        }

        population.se_sort_population();

        if (population.population[0]->fitness1 < population.se_config.target_fitness1) {
            population.se_early_exit(i);
            break;
        }
    }

    population.se_calculate_fitness2();
    population.se_log_statistics();

    return result;
}
}