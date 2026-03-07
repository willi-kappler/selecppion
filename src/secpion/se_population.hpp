/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for one population
*/

#ifndef FILE_SE_POPULATION_HPP_INCLUDED
#define FILE_SE_POPULATION_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <string_view>
#include <stdfloat>
#include <memory>

// External includes:
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <nodcru2/nc_logger.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_individual.hpp"
#include "se_random.hpp"

namespace secpion {
template<typename T>
class SEPopulation {
    public:
        SEConfiguration se_config;
        std::shared_ptr<spdlog::logger> se_logger;

        std::vector<std::unique_ptr<SEIndividual>> population;
        size_t best_index;
        size_t worst_index;
        size_t mut_op_index;
        bool minimum_found;
        T rng;
        uint64_t randomize_iteration;

        SEPopulation(SEConfiguration config):
            se_config(config),
            se_logger(),
            population(),
            best_index(0),
            worst_index(0),
            mut_op_index(0),
            minimum_found(false),
            rng(),
            randomize_iteration(0)
        {
            rng.seed();
            spdlog::drop("se_logger");
            se_logger = spdlog::stdout_logger_mt("se_logger");
        }

        void se_set_logger(std::shared_ptr<spdlog::logger> logger) {
            se_logger = logger;

            se_logger->debug("Population size: {}, target fitness 1: {}, target fitness 2: {}",
                se_config.node_population_size, se_config.target_fitness1, se_config.target_fitness2);
            se_logger->debug("Number of iterations: {}, number of mutations: {}",
                se_config.num_of_iterations, se_config.num_of_mutations);
            se_logger->debug("Randomize population: {}, randomize count: {}",
                se_config.randomize_population, se_config.randomize_count);
            se_logger->debug("Accept new best: {}", se_config.accept_new_best);
            se_logger->debug("Mutation operations: {}", se_config.mutation_operations);
        }

        void se_set_loglevel(spdlog::level::level_enum level) {
            se_logger->set_level(level);
        }

        void se_set_file_logger(std::string_view prefix) {
            std::string file_name = nodcru2::nc_gen_log_file_name(prefix);
            spdlog::drop("se_logger");
            std::shared_ptr<spdlog::logger> file_logger = spdlog::basic_logger_mt("se_logger", file_name);
            se_set_logger(file_logger);
        }

        void se_fill_population(std::unique_ptr<SEIndividual> individual) {
            size_t size = se_config.node_population_size;
            population.reserve(size);
            std::unique_ptr<SEIndividual> new_indi;

            for (size_t i = 0; i < size; i++) {
                new_indi = individual->se_clone();
                new_indi->se_randomize();
                new_indi->se_calculate_fitness1();
                population.push_back(std::move(new_indi));
            }
        }

        void se_find_worst_individual() {
            worst_index = 0;
            std::float64_t worst_fitness = population[0]->fitness1;
            std::float64_t fitness = 0.0;

            for (size_t i = 1; i < population.size(); i++) {
                fitness = population[i]->fitness1;

                if (fitness > worst_fitness) {
                    worst_index = i;
                    worst_fitness = fitness;
                }
            }
        }

        void se_find_best_and_worst_individual() {
            worst_index = 0;
            best_index = 0;
            std::float64_t worst_fitness = population[0]->fitness1;
            std::float64_t best_fitness = population[0]->fitness1;
            std::float64_t fitness = 0.0;

            for (size_t i = 1; i < population.size(); i++) {
                fitness = population[i]->fitness1;

                if (fitness > worst_fitness) {
                    worst_index = i;
                    worst_fitness = fitness;
                } else if (fitness < best_fitness) {
                    best_index = i;
                    best_fitness = fitness;
                }
            }
        }

        void se_sort_population() {
            std::sort(population.begin(), population.end(),
                [] (const std::unique_ptr<SEIndividual>&ind1, const std::unique_ptr<SEIndividual>&ind2) {
                    return ind1->fitness1 < ind2->fitness1; });
        }

        void se_random_population() {
            for (auto &individual: population) {
                individual->se_reset_counter();
                individual->se_randomize();
                individual->se_calculate_fitness1();
            }
        }

        void se_randomize_or_accept_best(std::span<const uint8_t> data) {
            if (se_config.randomize_population) {
                randomize_iteration++;
                if (randomize_iteration > se_config.randomize_count) {
                    randomize_iteration = 0;
                    se_logger->debug("Randomize population.");
                    se_random_population();
                }
            } else if (se_config.accept_new_best) {
                se_logger->debug("Accept new best from server.");
                population[0]->se_from_span_u8(data);
                se_logger->debug("Fitness from server: {}", population[0]->fitness1);
            }
        }

        void se_shuffle_mutation_operations() {
            se_logger->debug("Shuffle mutation operations.");
            rng.shuffle(se_config.mutation_operations);
        }

        void se_randomize_worst() {
            population[worst_index]->se_randomize();
            population[worst_index]->se_calculate_fitness1();
            // Now it may not be the worst anymore!
        }

        void se_replace_best(std::unique_ptr<SEIndividual> individual) {
            if (individual->fitness1 < population[best_index]->fitness1) {
                population[best_index] = std::move(individual);
            }
        }

        void se_replace_worst(std::unique_ptr<SEIndividual> individual) {
            population[worst_index] = std::move(individual);
            // No it may no longer be the worst anymore!
        }

        void se_clone_best_to_worst() {
            population[worst_index] = std::move(population[best_index]->se_clone_internal());
        }

        [[nodiscard]] const SEIndividual& se_get_best() const {
            return *population[best_index];
        }

        [[nodiscard]] std::float64_t se_get_best_fitness() {
            return population[best_index]->fitness1;
        }

        [[nodiscard]] std::float64_t se_get_worst_fitness() {
            return population[worst_index]->fitness1;
        }

        [[nodiscard]] uint8_t se_get_mut_op() {
            mut_op_index++;

            if (mut_op_index >= se_config.mutation_operations.size()) {
                mut_op_index = 0;
            }

            return se_config.mutation_operations[mut_op_index];
        }

        void se_check_limit(std::unique_ptr<SEIndividual> individual, std::float64_t limit, size_t i) {
            if ((individual->fitness1 < limit) || (individual->fitness1 < population[i]->fitness1)) {
                population[i] = std::move(individual);
            }
        }

        void se_early_exit(uint64_t iteration) {
            se_logger->info("Early exit at iteration: {}", iteration);
            minimum_found = true;

            if (iteration == 0) {
                // Wait some seconds to avoid spamming the server
                auto const sleep_time = std::chrono::seconds(se_config.early_exit_sleep);
                std::this_thread::sleep_for(sleep_time);
            }
        }

        void se_calculate_fitness2() {
            se_logger->debug("Calculate fitness 2.");
            std::float64_t best_fitness2 = SE_FLOAT_MAX;
            std::float64_t fitness2 = 0.0;
            size_t best_fitness2_index = 0;

            for (size_t i = 0; i < population.size(); i++) {
                if (population[i]->fitness1 < 0.01) {
                    population[i]->se_calculate_fitness2();
                    fitness2 = population[i]->fitness2;

                    if (fitness2 < best_fitness2) {
                        best_fitness2 = fitness2;
                        best_fitness2_index = i;
                    }
                }
            }

            if (best_fitness2 < SE_FLOAT_MAX) {
                if (best_fitness2_index != best_index) {
                    se_logger->debug("Best index: {}, new best index: {}",
                        best_index, best_fitness2_index);
                    se_logger->debug("Best fitness 1: {}, best fitness 2: {}",
                        population[best_index]->fitness1, population[best_index]->fitness2);
                    se_logger->debug("New best fitness 1: {}, new best fitness 2: {}",
                        population[best_fitness2_index]->fitness1, population[best_fitness2_index]->fitness2);
                    best_index = best_fitness2_index;
                }
            }
        }

        void se_log_statistics() {
            se_logger->debug("Best fitness 1: {}, best fitness 2: {}",
                population[best_index]->fitness1, population[best_index]->fitness2);
            se_logger->debug("Worst fitness 1: {}, worst fitness 2: {}",
                population[worst_index]->fitness1, population[worst_index]->fitness2);
            se_logger->debug("Actual best: {}, actual worst: {}",
                population[best_index]->se_actual_fitness(), population[worst_index]->se_actual_fitness());
            se_logger->debug("Best mutations: {}", population[best_index]->mut_op_counter);
            se_logger->debug("Worst mutations: {}", population[worst_index]->mut_op_counter);
        }
};
}

#endif // FILE_SE_POPULATION_HPP_INCLUDED