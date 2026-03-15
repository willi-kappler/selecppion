/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 9.
*/

#ifndef FILE_SE_POPULATION_NODE9_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE9_HPP_INCLUDED

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

namespace secpion {
template<typename T>
class SEPopulationNode9: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode9(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN9: Process data.", data);
            std::unique_ptr<SEIndividual> cloned_indi;
            std::float64_t current_limit;

            for (uint32_t i = 0; i < population.se_config.num_of_iterations; i++) {
                current_limit = population.se_config.sine_base + (population.se_config.sine_amplitude *
                    sin(population.se_config.sine_frequency * static_cast<std::float64_t>(i)));
                for (size_t j = 0; j < population.se_config.node_population_size; j++) {
                    cloned_indi = population.population[j]->se_clone_internal();

                    for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                        cloned_indi->se_mutate_internal(population.se_get_mut_op());
                    }

                    cloned_indi->se_calculate_fitness1();

                    population.se_check_limit(std::move(cloned_indi), current_limit, j);

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
            population.se_logger->info("Population type 9.");
            population.se_logger->info("Use sine wave to set the fitness limit.");
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

typedef SEPopulationNode9<SE_RNG_L64> SEPop9_L64;
}

#endif // FILE_SE_POPULATION_NODE9_HPP_INCLUDED