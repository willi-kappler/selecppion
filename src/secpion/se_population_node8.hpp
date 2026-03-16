/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 8.
*/

#ifndef FILE_SE_POPULATION_NODE8_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE8_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>

// External includes:
#include <nodcru2/nc_node.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_population.hpp"

using namespace nodcru2;

namespace secpion {
template<typename T>
class SEPopulationNode8: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode8(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));
            population.best_index = 0;
            population.worst_index = population.se_config.node_population_size - 1;

            population.se_logger->info("Population type 8.");
            population.se_logger->info("Always clone only the best one.");
            population.se_logger->info("If mutation is better than the best, overwrite.");
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN8: Process data.", data);
            population.se_sort_population();
            std::unique_ptr<SEIndividual> cloned_indi;

            for (uint32_t i = 0; i < population.se_config.num_of_iterations; i++) {
                // Overwrite all other individuals from population with the best one:
                for (size_t j = 1; j < population.se_config.node_population_size; j++) {
                    population.population[j] = population.population[0]->se_clone_internal();
                }

                for (size_t j = 1; j < population.se_config.node_population_size; j++) {
                    for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                        population.population[j]->se_mutate_internal(population.se_get_mut_op());
                        population.population[j]->se_calculate_fitness1();

                        if (population.population[j]->fitness1 < population.population[0]->fitness1) {
                            population.population[0] = population.population[j]->se_clone_internal();
                        }
                    }

                    if (population.population[0]->fitness1 <= population.se_config.target_fitness1) {
                        population.se_early_exit(i);
                        break;
                    }
                }

                if (population.minimum_found) {
                    break;
                }
            }

            population.se_sort_population();
            population.se_calculate_fitness2();
            population.se_log_statistics();

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

typedef SEPopulationNode8<SE_RNG_L64> SEPop8_L64;
}

#endif // FILE_SE_POPULATION_NODE8_HPP_INCLUDED