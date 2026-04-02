/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 10.
*/

#ifndef FILE_SE_POPULATION_NODE10_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE10_HPP_INCLUDED

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
class SEPopulationNode10: public NCNodeDataProcessor {
    private:
        SEPopulation<T> population;

    public:
        SEPopulationNode10(SEConfiguration se_config, std::unique_ptr<SEIndividual> individual):
            NCNodeDataProcessor(),
            population(se_config) {
            population.se_fill_population(std::move(individual));

            population.se_logger->info("Population type 10.");
            population.se_logger->info("Always replace the worst individual if better fitness1.");
        }

        void nc_init([[maybe_unused]] std::vector<uint8_t> data, NCNodeID node_id) override {
            population.se_logger->info("Current node id: {}", node_id);
        }

        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override {
            population.se_prepare_iteration("PN10: Process data.", data);
            population.se_find_worst_individual();

            std::unique_ptr<SEIndividual> cloned_indi;
            size_t j = 0;

            for (uint32_t i = 0; i < population.se_config.num_of_iterations * population.se_config.node_population_size; i++) {
                j = population.rng.get_size_t(population.se_config.node_population_size);

                if (population.population[j]->fitness1 <= population.se_config.target_fitness1) {
                    population.se_early_exit(i);
                    break;
                }

                cloned_indi = population.population[j]->se_clone_internal();

                for (uint32_t k = 0; k < population.se_config.num_of_mutations; k++) {
                    cloned_indi->se_mutate_internal(population.se_get_mut_op());
                }

                cloned_indi->se_calculate_fitness1();

                if (cloned_indi->fitness1 < population.se_get_worst_fitness()) {
                    population.se_replace_worst(std::move(cloned_indi));
                    population.se_find_worst_individual();
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

typedef SEPopulationNode10<SE_RNG_L64> SEPop10_L64;
}

#endif // FILE_SE_POPULATION_NODE10_HPP_INCLUDED