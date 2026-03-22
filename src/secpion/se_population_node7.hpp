/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 7.
*/

#ifndef FILE_SE_POPULATION_NODE7_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE7_HPP_INCLUDED

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
class SEPopulationNode7: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode7(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));
            population.best_index = 0;
            population.worst_index = population.se_config.node_population_size - 1;

            population.se_logger->info("Population type 7.");
            population.se_logger->info("Best individual at index 0. Increase factor with index.");
            population.se_logger->info("Set limit based on factor and best fitness.");
            population.se_logger->info("Limit factor: {}", se_config.limit_factor);
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN7: Process data.", data);
            std::float64_t current_best_fitness = population.population[0]->fitness1;
            std::float64_t fitness_limit = 0.0;
            std::unique_ptr<SEIndividual> cloned_indi;

            for (uint32_t i = 0; i < population.se_config.num_of_iterations; i++) {
                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    cloned_indi = population.population[j]->se_clone_internal();

                    for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                        cloned_indi->se_mutate_internal(population.se_get_mut_op());
                    }
                    cloned_indi->se_calculate_fitness1();

                    if (cloned_indi->fitness1 < current_best_fitness) {
                        current_best_fitness = cloned_indi->fitness1;
                        population.population[0] = std::move(cloned_indi);
                    } else {
                        if (j > 0) {
                            fitness_limit = current_best_fitness *
                                (pow(population.se_config.limit_factor, static_cast<std::float64_t>(j)));
                            population.se_check_limit(std::move(cloned_indi), fitness_limit, j);
                        }
                    }

                }

                if (current_best_fitness <= population.se_config.target_fitness1) {
                    population.se_early_exit(i);
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

typedef SEPopulationNode7<SE_RNG_L64> SEPop7_L64;
}

#endif // FILE_SE_POPULATION_NODE7_HPP_INCLUDED