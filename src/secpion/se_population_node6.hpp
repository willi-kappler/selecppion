/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 6.
*/

#ifndef FILE_SE_POPULATION_NODE6_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE6_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <memory>

// External includes:
#include <nodcru2/nc_node.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_population.hpp"

using namespace nodcru2;

namespace secpion {
template<typename T>
class SEPopulationNode6: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

        /*
        void log_fitness3(size_t i, size_t j, const char block, std::float64_t indi1_fitness, std::float64_t indi2_fitness, std::float64_t indi3_fitness) {
            if (j == 0) {
                if (i < 5) {
                    population.se_logger->debug("{}: {}, {}, {}", block, indi1_fitness, indi2_fitness, indi3_fitness);
                }
            }
        }

        void log_fitness2(size_t i, size_t j, const char block, std::float64_t indi1_fitness, std::float64_t indi2_fitness) {
            if ((i == 0) && (j == 0)) {
                population.se_logger->debug("{}: {}, {}", block, indi1_fitness, indi2_fitness);
            }
        }
        */

    public:
        SEPopulationNode6(SEConfiguration se_config, const SEIndividual &individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(individual);

            population.se_logger->info("Population type 6.");
            population.se_logger->info("Clone two individuals for each individual in the population.");
            population.se_logger->info("After each mutation, calculate fitness and keep if better.");
            population.se_logger->info("The first clone keeps mutating, the second clone is reset to the initial individual.");
            population.se_logger->info("The best of all the mutations is kept and the next individual is mutated.");
        }

        void nc_init([[maybe_unused]] std::vector<uint8_t> data, NCNodeID node_id) override {
            population.se_logger->info("Current node id: {}", node_id.to_string());
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN6: Process data.", data);

            std::unique_ptr<SEIndividual> cloned_indi1 = population.population[0]->se_clone_internal();
            std::unique_ptr<SEIndividual> cloned_indi2 = population.population[0]->se_clone_internal();
            std::unique_ptr<SEIndividual> cloned_indi3 = population.population[0]->se_clone_internal();
            const uint32_t num_of_iterations = population.se_config.num_of_iterations;

            // Copy the best to all other individuals.
            population.se_sort_population();
            for (size_t i = 1; i < population.se_config.node_population_size; i++) {
                population.population[i]->se_copy_from_internal(*population.population[0]);
            }

            for (uint32_t i = 0; i < num_of_iterations; i++) {
                population.se_sub_iteration(i);

                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    cloned_indi1->se_copy_from_internal(*population.population[j]);
                    cloned_indi2->se_copy_from_internal(*population.population[j]);
                    cloned_indi3->se_copy_from_internal(*population.population[j]);

                    // log_fitness3(i, j, 'A', cloned_indi1->fitness1, cloned_indi2->fitness1, cloned_indi3->fitness1);

                    for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                        cloned_indi1->se_mutate_internal(population.se_get_mut_op());
                        cloned_indi1->se_calculate_fitness1();

                        if (cloned_indi1->fitness1 < cloned_indi3->fitness1) {
                            cloned_indi3->se_copy_from_internal(*cloned_indi1);
                        }

                        cloned_indi2->se_mutate_internal(population.se_get_mut_op());
                        cloned_indi2->se_calculate_fitness1();

                        if (cloned_indi2->fitness1 < cloned_indi3->fitness1) {
                            cloned_indi3->se_copy_from_internal(*cloned_indi2);
                        }

                        cloned_indi2->se_copy_from_internal(*population.population[j]);
                    }

                    // log_fitness3(i, j, 'B', cloned_indi1->fitness1, cloned_indi2->fitness1, cloned_indi3->fitness1);

                    if (cloned_indi3->fitness1 < population.population[j]->fitness1) {
                        population.population[j]->se_copy_from_internal(*cloned_indi3);
                    }

                    if (population.population[j]->fitness1 <= population.se_config.target_fitness1) {
                        population.se_early_exit(i);
                        break;
                    }
                }

                if (population.minimum_found) {
                    break;
                }
            }

            population.se_find_best_and_worst_individual();
            population.se_calculate_fitness2();
            population.se_log_statistics();
            // population.se_fill_with_best();

            population.se_get_best()->se_set_metadata(std::format("PN6, max_iter: {}, mut: {}",
                population.se_config.num_of_iterations, population.se_config.num_of_mutations));

            return population.se_get_best()->se_to_vec_u8();
        }

        // These two methods are used for the test cases:
        [[nodiscard]] SEIndividual* se_get_individual(size_t i) {
            return population.population[i].get();
        }

        [[nodiscard]] SEIndividual* se_get_worst() {
            return population.population[population.worst_index].get();
        }

        void se_set_logger(std::shared_ptr<spdlog::logger> logger) {
            population.se_set_logger(logger);
        }
};

typedef SEPopulationNode6<SE_RNG_L64> SEPop6_L64;
}

#endif // FILE_SE_POPULATION_NODE6_HPP_INCLUDED
