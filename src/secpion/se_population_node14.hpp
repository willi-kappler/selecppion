/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 14.
*/

#ifndef FILE_SE_POPULATION_NODE14_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE14_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <memory>
#include <format>

// External includes:
#include <nodcru2/nc_node.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_population.hpp"

using namespace nodcru2;

namespace secpion {
template<typename T>
class SEPopulationNode14: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode14(SEConfiguration se_config, const SEIndividual &individual):
            NCNodeDataProcessor(),
            population(se_config)
            {
                population.se_fill_population(individual);
                population.se_sort_population();
                population.best_index = 0;
                population.worst_index = population.se_config.node_population_size - 1;

                population.se_logger->info("Population type 14.");
                population.se_logger->info("Clone the best one a few times before iteration and mutate the clones.");
                population.se_logger->info("Then clone and mutate each individual separatly and if it's better, keep it.");
            }

        void nc_init([[maybe_unused]] std::vector<uint8_t> data, NCNodeID node_id) override {
            population.se_logger->info("Current node id: {}", node_id.to_string());
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN14: Process data.", data);
            std::unique_ptr<SEIndividual> cloned_indi = population.population[0]->se_clone_internal();
            const uint32_t num_of_iterations = population.se_config.num_of_iterations;
            const size_t population_size = population.se_config.node_population_size;
            size_t i = 0, j = 0, k = 0;
            uint8_t clone_counter = 0;

            // Prepare the individuals:
            for (i = 1; i < population_size; i++) {
                for (j = 0; j < population.se_config.num_of_mutations; j++) {
                    cloned_indi->se_mutate_internal(population.se_get_mut_op());
                }

                cloned_indi->se_calculate_fitness1_internal();

                population.population[i]->se_copy_from_internal(*cloned_indi);

                clone_counter++;

                if (clone_counter >= 20) {
                    clone_counter = 0;
                    cloned_indi->se_copy_from_internal(*population.population[0]);
                }
            }

            // Do the actual iteration:
            for (i = 0; i < num_of_iterations; i++) {
                population.se_sub_iteration(i);

                for (j = 0; j < population_size; j++) {
                    cloned_indi->se_copy_from_internal(*population.population[j]);

                    for (k = 0; k < population.se_config.num_of_mutations; k++) {
                        cloned_indi->se_mutate_internal(population.se_get_mut_op());
                    }

                    cloned_indi->se_calculate_fitness1_internal();

                    if (cloned_indi->fitness1 < population.population[j]->fitness1) {
                        population.population[j]->se_copy_from_internal(*cloned_indi);

                        if (cloned_indi->fitness1 <= population.se_config.target_fitness1) {
                            population.se_early_exit(i);
                            break;
                        }
                    }
                }

                if (population.minimum_found) {
                    break;
                }
            }

            population.se_sort_population();

            population.best_index = 0;
            population.se_calculate_fitness2();
            population.se_log_statistics();
            population.se_get_best()->se_set_metadata(std::format("PN14, max_iter: {}, mut: {}",
                population.se_config.num_of_iterations, population.se_config.num_of_mutations));

            return population.se_get_best()->se_to_vec_u8();
        }

        // These two methods are used for the test cases:
        [[nodiscard]] SEIndividual* se_get_individual(size_t i) {
            return population.population[i].get();
        }

        [[nodiscard]] SEIndividual* se_get_worst() {
            return population.population[population.se_config.node_population_size - 1].get();
        }

        void se_set_logger(std::shared_ptr<spdlog::logger> logger) {
            population.se_set_logger(logger);
        }
};

typedef SEPopulationNode14<SE_RNG_L64> SEPop14_L64;
}

#endif // FILE_SE_POPULATION_NODE14_HPP_INCLUDED
