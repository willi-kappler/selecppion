/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 2.
*/

#ifndef FILE_SE_POPULATION_NODE2_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE2_HPP_INCLUDED

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
class SEPopulationNode2: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode2(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));
            population.se_sort_population();
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_logger->info("PN2: Process data.");
            population.rng.seed();
            population.se_randomize_or_accept_best(data);
            population.se_shuffle_mutation_operations();

            std::unique_ptr<SEIndividual> cloned_indi;

            for (size_t i = 0; i < population.se_config.num_of_iterations; i++) {
                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    cloned_indi = population.population[j]->se_clone_internal();

                    for (uint32_t i = 0; i < population.se_config.num_of_mutations; i++) {
                        cloned_indi->se_mutate_internal(population.se_get_mut_op());
                    }

                    cloned_indi->se_calculate_fitness1();

                    // Only replace with clone if better then original:
                    if (cloned_indi->fitness1 < population.population[j]->fitness1) {
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

        void se_log_info() {
            population.se_logger->info("Population type 2.");
            population.se_logger->info("Mutate a clone and if it's better than the previous version keep it.");
        }

        void se_set_logger(std::shared_ptr<spdlog::logger> logger) {
            population.se_set_logger(logger);
            se_log_info();
        }

        void se_set_loglevel(spdlog::level::level_enum level) {
            population.se_set_loglevel(level);
        }

        void se_set_file_logger(std::string_view prefix) {
            population.se_set_file_logger(prefix);
            se_log_info();
        }
};

typedef SEPopulationNode2<SE_RNG_L64> SEPop2_L64;
}

#endif // FILE_SE_POPULATION_NODE2_HPP_INCLUDED