/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 3.
*/

#ifndef FILE_SE_POPULATION_NODE3_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE3_HPP_INCLUDED

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
class SEPopulationNode3: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode3(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_logger->info("PN3: Process data.");
            population.rng.seed();
            population.se_randomize_or_accept_best(data);
            population.se_shuffle_mutation_operations();
            population.se_find_best_and_worst_individual();

            std::unique_ptr<SEIndividual> cloned_indi;
            uint32_t max_iter = population.se_config.num_of_iterations *  population.se_config.node_population_size;
            size_t j = 0;

            for (uint32_t i = 0; i < max_iter; i++) {
                j = population.rng.get_size_t(population.se_config.node_population_size);
                cloned_indi = population.population[j]->se_clone_internal();

                for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                    cloned_indi->se_mutate_internal(population.se_get_mut_op());
                }

                cloned_indi->se_calculate_fitness1();

                if (cloned_indi->fitness1 < population.se_get_best_fitness()) {
                    population.se_replace_best(std::move(cloned_indi));
                } else if (cloned_indi->fitness1 < population.se_get_worst_fitness()) {
                    population.se_replace_worst(std::move(cloned_indi));
                    population.se_find_worst_individual();
                }

                if (population.se_get_best_fitness() <= population.se_config.target_fitness1) {
                    population.se_early_exit(i);
                    break;
                }
            }

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
            population.se_logger->info("Population type 3.");
            population.se_logger->info("Randomly pick an individual and mutate it.");
            population.se_logger->info("If it's better than the best replace it.");
            population.se_logger->info("Else if it's better than the worst replace it.");
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

typedef SEPopulationNode3<SE_RNG_L64> SEPop3_L64;
}

#endif // FILE_SE_POPULATION_NODE3_HPP_INCLUDED