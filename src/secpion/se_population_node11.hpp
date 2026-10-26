/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 11.
*/

#ifndef FILE_SE_POPULATION_NODE11_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE11_HPP_INCLUDED

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
class SEPopulationNode11: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode11(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));

            population.se_logger->info("Population type 11.");
            population.se_logger->info("Randomly select one individual to maybe mutate.");
            population.se_logger->info("Randomly select another individual to maybe crossover.");
            population.se_logger->info("If the new fitness is better than the worst replace it.");
            population.se_logger->info("Mutation probability: {}, crossover probability: {}",
                se_config.mutation_probability, se_config.crossover_probability);
        }

        void nc_init([[maybe_unused]] std::vector<uint8_t> data, NCNodeID node_id) override {
            population.se_logger->info("Current node id: {}", node_id.to_string());
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN11: Process data.", data);
            population.se_find_worst_individual();

            std::unique_ptr<SEIndividual> new_indi;
            const uint32_t num_of_iterations = population.se_config.num_of_iterations;
            size_t k1 = 0;
            size_t k2 = 0;
            bool exit_iteration = false;

            for (size_t i = 0; i < num_of_iterations; i++) {
                population.population[0]->se_current_iteration(i, num_of_iterations);

                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    k1 = population.rng.get_size_t(population.se_config.node_population_size);

                    if (population.population[k1]->fitness1 <= population.se_config.target_fitness1) {
                        exit_iteration = true;
                        break;
                    }

                    if (population.rng.get_float64() <= population.se_config.mutation_probability) {
                        for (uint32_t l = 0; l < population.se_config.num_of_mutations; l++) {
                            population.population[k1]->se_mutate_internal(population.se_get_mut_op());
                        }

                        population.population[k1]->se_calculate_fitness1();
                    }

                    if (population.rng.get_float64() <= population.se_config.crossover_probability) {
                        k2 = population.rng.get_size_t(population.se_config.node_population_size);

                        while (k1 == k2) {
                            k2 = population.rng.get_size_t(population.se_config.node_population_size);
                        }

                        new_indi = population.population[k1]->se_crossover(population.population[k2].get());
                        new_indi->se_calculate_fitness1();

                        if (new_indi->fitness1 < population.se_get_worst_fitness()) {
                            population.se_replace_worst(std::move(new_indi));
                            population.se_find_worst_individual();
                        }
                    }
                }

                if (exit_iteration) {
                    population.se_early_exit(i);
                    break;
                }
            }

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

typedef SEPopulationNode11<SE_RNG_L64> SEPop11_L64;
}

#endif // FILE_SE_POPULATION_NODE11_HPP_INCLUDED
