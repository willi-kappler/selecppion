/*
    Selecppion
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

namespace secpion {
template<typename T>
class SEPopulationNode6: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode6(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_logger->info("PN6: Process data.");
            population.rng.seed();
            population.se_randomize_or_accept_best(data);
            population.se_shuffle_mutation_operations();

            std::unique_ptr<SEIndividual> cloned_indi1;
            std::unique_ptr<SEIndividual> cloned_indi2;
            std::unique_ptr<SEIndividual> cloned_indi3;

            for (uint32_t i = 0; i < population.se_config.num_of_iterations; i++) {
                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    cloned_indi1 = population.population[j]->se_clone_internal();
                    cloned_indi2 = population.population[j]->se_clone_internal();
                    cloned_indi3 = population.population[j]->se_clone_internal();

                    for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                        cloned_indi1->se_mutate_internal(population.se_get_mut_op());
                        cloned_indi1->se_calculate_fitness1();

                        if (cloned_indi1->fitness1 < cloned_indi3->fitness1) {
                            cloned_indi3 = cloned_indi1->se_clone_internal();
                        }

                        cloned_indi2->se_mutate_internal(population.se_get_mut_op());
                        cloned_indi2->se_calculate_fitness1();

                        if (cloned_indi2->fitness1 < cloned_indi3->fitness1) {
                            cloned_indi3 = cloned_indi2->se_clone_internal();
                        }

                        cloned_indi2 = population.population[j]->se_clone_internal();
                    }

                    if (cloned_indi3->fitness1 < population.population[j]->fitness1) {
                        population.population[j] = std::move(cloned_indi3);
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

        void se_set_logger(std::shared_ptr<spdlog::logger> logger) {
            population.se_set_logger(logger);
            population.se_logger->info("Population type 6.");
            population.se_logger->info("Clone two individuals for each individual in the population.");
            population.se_logger->info("After each mutation, calculate fitness and keep if better.");
            population.se_logger->info("The first clone keeps mutating, the second clone is reset to the initial individual.");
            population.se_logger->info("The best of all the mutations is kept and the next individual is mutated.");
        }

        void se_set_loglevel(spdlog::level::level_enum level) {
            population.se_set_loglevel(level);
        }

        void se_set_file_logger(std::string_view prefix) {
            population.se_set_file_logger(prefix);
        }
};
}

#endif // FILE_SE_POPULATION_NODE6_HPP_INCLUDED