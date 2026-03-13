/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 4.
*/

#ifndef FILE_SE_POPULATION_NODE4_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE4_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <memory>

// External includes:
#include <nodcru2/nc_node.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_population.hpp"

namespace secpion {
template<typename T>
class SEPopulationNode4: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode4(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_logger->info("PN4: Process data.");
            population.rng.seed();
            population.se_randomize_or_accept_best(data);
            population.se_shuffle_mutation_operations();
            population.se_find_worst_individual();

            std::unique_ptr<SEIndividual> cloned_indi;
            std::float64_t global_fitness = population.se_get_worst_fitness();
            uint32_t indi_below_global_fitness;
            uint32_t all_above_global_fitness = 0;

            for (size_t i = 0; i < population.se_config.num_of_iterations; i++) {
                indi_below_global_fitness = 0;

                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    cloned_indi = population.population[j]->se_clone_internal();

                    for (uint32_t i = 0; i < population.se_config.num_of_mutations; i++) {
                        cloned_indi->se_mutate_internal(population.se_get_mut_op());
                    }

                    cloned_indi->se_calculate_fitness1();
                    population.se_check_limit(std::move(cloned_indi), global_fitness, j);

                    if (population.population[j]->fitness1 <= population.se_config.target_fitness1) {
                        population.se_early_exit(i);
                        break;
                    }

                    if (population.population[j]->fitness1 < global_fitness) {
                        indi_below_global_fitness++;
                    }
                }

                if (population.minimum_found) {
                    break;
                }

                if (indi_below_global_fitness >= population.se_config.min_num_of_individuals) {
                    global_fitness = global_fitness * 0.9;
                } else {
                    global_fitness = global_fitness * 1.01;
                    all_above_global_fitness++;
                }
            }

            population.se_logger->debug("All individual above global fitness: {}", all_above_global_fitness);

            population.se_find_best_and_worst_individual();
            population.se_calculate_fitness2();
            population.se_log_statistics();
            //population.se_clone_best_to_worst();

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
            population.se_logger->info("Population type 4.");
            population.se_logger->info("Use a global fitness that is the same for all individuals.");
            population.se_logger->info("Mutate an individual and if it's better than the global fitness keep it.");
            population.se_logger->info("Reduce global fitness each iteration.");
            population.se_logger->info("If no individual is better, increase the global fitness a bit.");
        }

        void se_set_loglevel(spdlog::level::level_enum level) {
            population.se_set_loglevel(level);
        }

        void se_set_file_logger(std::string_view prefix) {
            population.se_set_file_logger(prefix);
        }
};

typedef SEPopulationNode4<SE_RNG_L64> SEPop4_L64;
}

#endif // FILE_SE_POPULATION_NODE4_HPP_INCLUDED