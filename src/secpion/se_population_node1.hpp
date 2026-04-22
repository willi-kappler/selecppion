/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 1.
*/

#ifndef FILE_SE_POPULATION_NODE1_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE1_HPP_INCLUDED

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
class SEPopulationNode1: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;
        uint32_t offset;

    public:
        SEPopulationNode1(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config),
            offset(se_config.node_population_size / 2) {
            population.se_fill_population(std::move(individual));
            population.se_sort_population();
            population.best_index = 0;
            population.worst_index = population.se_config.node_population_size - 1;

            population.se_logger->info("Population type 1.");
            population.se_logger->info("Clone population and mutate individuals in place. Then sort population by fitness.");
            population.se_logger->info("The worst individuals are overwritten.");
        }

        void nc_init([[maybe_unused]] std::vector<uint8_t> data, NCNodeID node_id) override {
            population.se_logger->info("Current node id: {}", node_id);
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN1: Process data.", data);
            std::unique_ptr<SEIndividual> cloned_indi;

            for (size_t i = 0; i < population.se_config.num_of_iterations; i++) {
                for (size_t j = 0; j < offset; j++) {
                    // Create a copy of each individual before mutating it (lower half):
                    cloned_indi = population.population[j]->se_clone_internal();

                    for (uint32_t i = 0; i < population.se_config.num_of_mutations; i++) {
                        cloned_indi->se_mutate_internal(population.se_get_mut_op());
                    }

                    cloned_indi->se_calculate_fitness1();
                    // Overwrite upper half (the bad ones):
                    population.population[j + offset] = std::move(cloned_indi);
                }

                population.se_sort_population();

                if (population.population[0]->fitness1 <= population.se_config.target_fitness1) {
                    population.se_early_exit(i);
                    break;
                }
            }

            population.se_calculate_fitness2();
            population.se_log_statistics();

            return population.population[0]->se_to_vec_u8();
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

typedef SEPopulationNode1<SE_RNG_L64> SEPop1_L64;
}

#endif // FILE_SE_POPULATION_NODE1_HPP_INCLUDED
