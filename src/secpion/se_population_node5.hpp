/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 5.
*/

#ifndef FILE_SE_POPULATION_NODE5_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE5_HPP_INCLUDED

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
class SEPopulationNode5: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode5(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));

            population.se_logger->info("Population type 5.");
            population.se_logger->info("Before iterating calculate the average fitness.");
            population.se_logger->info("Mutate each individual and if better than the average replace it.");
            population.se_logger->info("Also lower (update) the average fitness as new bound.");
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN5: Process data.", data);

            std::unique_ptr<SEIndividual> cloned_indi;
            std::float64_t fitness_limit = population.se_get_average_fitness1();
            population.se_logger->debug("Fitness limit: {}", fitness_limit);

            for (uint32_t i = 0; i < population.se_config.num_of_iterations; i++) {
                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    cloned_indi = population.population[j]->se_clone_internal();

                    for (uint32_t i = 0; i < population.se_config.num_of_mutations; i++) {
                        cloned_indi->se_mutate_internal(population.se_get_mut_op());
                    }

                    cloned_indi->se_calculate_fitness1();
                    if (cloned_indi->fitness1 < fitness_limit) {
                        fitness_limit = (fitness_limit + cloned_indi->fitness1) / 2.0;
                        population.population[j] = std::move(cloned_indi);
                    } else if (cloned_indi->fitness1 < population.population[j]->fitness1) {
                        population.population[j] = std::move(cloned_indi);
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

            population.se_logger->debug("Fitness limit: {}", fitness_limit);

            population.se_find_best_and_worst_individual();
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

typedef SEPopulationNode5<SE_RNG_L64> SEPop5_L64;
}

#endif // FILE_SE_POPULATION_NODE5_HPP_INCLUDED