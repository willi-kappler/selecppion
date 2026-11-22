/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 13.
*/

#ifndef FILE_SE_POPULATION_NODE13_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE13_HPP_INCLUDED

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
class SEPopulationNode13: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;
        uint32_t offset;
        std::vector<std::unique_ptr<SEIndividual>> list_of_best;

    public:
        SEPopulationNode13(SEConfiguration se_config, const SEIndividual &individual):
            NCNodeDataProcessor(),
            population(se_config),
            offset(se_config.node_population_size / 10),
            list_of_best()
            {
                population.se_fill_population(individual);
                population.se_sort_population();
                population.best_index = 0;
                population.worst_index = population.se_config.node_population_size - 1;

                population.se_logger->info("Population type 13.");
                population.se_logger->info("Clone one tenth of the population and mutate all the other individuals in place.");
                population.se_logger->info("Then sort population by fitness. The worst individuals are overwritten.");
                population.se_logger->info("Keep the best in a list and copy them back before the start of the iteration.");
                population.se_logger->info("offset: {}", offset);
            }

        void nc_init([[maybe_unused]] std::vector<uint8_t> data, NCNodeID node_id) override {
            population.se_logger->info("Current node id: {}", node_id.to_string());
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN13: Process data.", data);
            const uint32_t num_of_iterations = population.se_config.num_of_iterations;
            const size_t population_size = population.se_config.node_population_size;
            // const size_t offset2 = population_size / 2;
            size_t i = 0, j = 0, k = 0;
            const size_t max_best_size = std::min<size_t>(20, population_size / 2);

            for (i = 0; i < list_of_best.size(); i++) {
                population.population[i + 1]->se_copy_from_internal(*list_of_best[i]);
            }

            for (i = 0; i < num_of_iterations; i++) {
                population.se_sub_iteration(i);

                // Copy the best individuals and overwrite the bad ones:
                for (j = 0; j < offset; j++) {
                    population.population[population_size - j - 1]->se_copy_from_internal(*population.population[j]);
                }

                // Mutate the population from offset to end:
                for (j = offset; j < population_size; j++) {
                    for (k = 0; k < population.se_config.num_of_mutations; k++) {
                        population.population[j]->se_mutate_internal(population.se_get_mut_op());
                    }

                    population.population[j]->se_calculate_fitness1_internal();
                }

                population.se_sort_population();

                if (population.population[0]->fitness1 <= population.se_config.target_fitness1) {
                    population.se_early_exit(i);
                    break;
                }
            }

            /*
            for (i = 0; i < num_of_iterations; i++) {
                population.se_sub_iteration(i);

                // Copy some random individuals and overwrite the bad ones:
                for (j = 0; j < offset; j++) {
                    k = population.rng.get_size_t(offset2);
                    population.population[population_size - j - 1]->se_copy_from_internal(*population.population[k]);
                }

                // Mutate the rest:
                for (j = 0; j < offset2; j++) {
                    for (k = 0; k < population.se_config.num_of_mutations; k++) {
                        population.population[j]->se_mutate_internal(population.se_get_mut_op());
                    }

                    population.population[j]->se_calculate_fitness1_internal();
                }

                population.se_sort_population();

                if (population.population[0]->fitness1 <= population.se_config.target_fitness1) {
                    population.se_early_exit(i);
                    break;
                }
            }
            */


            population.best_index = 0;
            population.se_calculate_fitness2();
            population.se_log_statistics();
            population.se_get_best()->se_set_metadata(std::format("PN13, max_iter: {}, mut: {}",
                population.se_config.num_of_iterations, population.se_config.num_of_mutations));

            // Insert new best at the beginning:
            auto list_begin = list_of_best.begin();
            list_of_best.insert(list_begin, population.se_get_best()->se_clone_internal());

            if (list_of_best.size() > max_best_size) {
                // Remove the last element:
                list_of_best.resize(max_best_size);
            }

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

typedef SEPopulationNode13<SE_RNG_L64> SEPop13_L64;
}

#endif // FILE_SE_POPULATION_NODE13_HPP_INCLUDED
