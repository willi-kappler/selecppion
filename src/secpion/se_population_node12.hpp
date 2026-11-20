/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 12.
*/

#ifndef FILE_SE_POPULATION_NODE12_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE12_HPP_INCLUDED

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
class SEPopulationNode12: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode12(SEConfiguration se_config, const SEIndividual &individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(individual);
            population.se_sort_population();

            population.se_logger->info("Population type 12.");
            population.se_logger->info("Mutate a clone and if it's better than the previous version keep it.");
            population.se_logger->info("Repeat until the best fitness1 factor is greater than 1.5.");
            }

        void nc_init([[maybe_unused]] std::vector<uint8_t> data, NCNodeID node_id) override {
            population.se_logger->info("Current node id: {}", node_id.to_string());
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN12: Process data.", data);
            std::unique_ptr<SEIndividual> cloned_indi = population.population[0]->se_clone_internal();
            const uint32_t num_of_iterations = population.se_config.num_of_iterations;
            const uint32_t population_size = population.se_config.node_population_size;

            std::float64_t current_fitness1_factor = 0.0;
            // uint8_t factor_counter = 0;
            size_t index1, index2;

            while (true) {
                index1 = population.rng.get_size_t(population.se_config.node_population_size);
                index2 = population.rng.get_size_t(population.se_config.node_population_size);

                population.se_logger->debug("index1: {}, index2: {}", index1, index2);

                population.population[index1]->se_reset_counter();
                population.population[index1]->se_randomize();
                population.population[index1]->se_calculate_fitness1();

                for (uint32_t i = 0; i < population.se_config.mutate_before_iteration; i++) {
                    population.population[index2]->se_mutate_internal(population.se_get_mut_op());
                }
                population.population[index2]->se_calculate_fitness1();

                for (size_t i = 0; i < num_of_iterations; i++) {
                    population.se_sub_iteration(i);

                    for (size_t j = 0; j < population_size; j++) {
                        cloned_indi->se_copy_from_internal(*population.population[j]);

                        for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                            cloned_indi->se_mutate_internal(population.se_get_mut_op());
                        }

                        cloned_indi->se_calculate_fitness1_internal();

                        // Only replace with clone if better then original:
                        if (cloned_indi->fitness1 < population.population[j]->fitness1) {
                            population.population[j]->se_copy_from_internal(*cloned_indi);
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

                if (population.minimum_found) {
                    break;
                }

                population.se_find_best_and_worst_individual();
                current_fitness1_factor = population.fitness1_before_iteration / population.se_get_best_fitness();
                population.se_logger->debug("Current fitness1 factor: {}, best_index: {}", current_fitness1_factor, population.best_index);
                population.se_logger->flush();

                if (current_fitness1_factor >= 1.5) {
                    break;
                }/* else {
                    factor_counter++;

                    if (factor_counter >= 20) {
                        break;
                    }
                }*/
            }

            population.se_calculate_fitness2();
            population.se_log_statistics();

            population.se_get_best()->se_set_metadata(std::format("PN12, max_iter: {}, mut: {}",
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

typedef SEPopulationNode12<SE_RNG_L64> SEPop12_L64;
}

#endif // FILE_SE_POPULATION_NODE12_HPP_INCLUDED
