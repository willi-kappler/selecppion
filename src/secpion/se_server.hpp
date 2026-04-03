/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the server
*/

#ifndef FILE_SE_SERVER_HPP_INCLUDED
#define FILE_SE_SERVER_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <string_view>
#include <stdfloat>
#include <memory>
#include <fstream>
#include <unordered_map>

// External includes:
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/stopwatch.h>
#include <nodcru2/nc_util.hpp>
#include <nodcru2/nc_server.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_individual.hpp"
#include "se_random.hpp"
#include "se_exceptions.hpp"

using namespace nodcru2;

namespace secpion {
template<typename T>
class SEServerDP: public NCServerDataProcessor {
    private:
        SEConfiguration se_config;
        std::shared_ptr<spdlog::logger> se_logger;
        std::vector<std::unique_ptr<SEIndividual>> population;
        T rng;
        spdlog::stopwatch sw;
        uint32_t new_fitness_counter;
        std::unordered_map<NCNodeID, uint32_t> node_stats;

    public:
        SEServerDP(SEConfiguration config, std::unique_ptr<SEIndividual> individual):
            se_config(config),
            se_logger(),
            population(),
            rng(),
            sw(),
            new_fitness_counter(0),
            node_stats()
        {
            rng.seed();
            spdlog::drop("se_logger");

            if (se_config.se_server_log_file.size() > 0) {
                std::string file_name = nodcru2::nc_gen_log_file_name(se_config.se_server_log_file);
                se_logger = spdlog::basic_logger_mt("se_logger", file_name);
            } else {
                se_logger = spdlog::stdout_logger_mt("se_logger");
            }

            if (se_config.se_server_log_level == "debug") {
                se_logger->set_level(spdlog::level::level_enum::debug);
            } else if ((se_config.se_server_log_level == "info") || (se_config.se_server_log_level.size() == 0)) {
                se_logger->set_level(spdlog::level::level_enum::info);
            } else if (se_config.se_server_log_level == "warn") {
                se_logger->set_level(spdlog::level::level_enum::warn);
            } else if (se_config.se_server_log_level == "error") {
                se_logger->set_level(spdlog::level::level_enum::err);
            } else {
                throw SEConfigurationException(fmt::format("Unknown log level: {}", se_config.se_server_log_level).c_str());
            }

            se_fill_population(std::move(individual));
            se_sort_population();
        }

        void se_set_logger(std::shared_ptr<spdlog::logger> logger) {
            se_logger = logger;

            se_logger->info("Population size: {}, target fitness 1: {}, target fitness 2: {}",
                se_config.server_population_size, se_config.target_fitness1, se_config.target_fitness2);
            se_logger->info("Result filename: {}, save new fitness: {}",
                se_config.result_filename, se_config.save_new_fitness);
            se_logger->info("Allow same fitness: {}, share only best: {}",
                se_config.allow_same_fitness, se_config.share_only_best);
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
            size_t size = se_config.server_population_size;
            population.reserve(size);
            std::unique_ptr<SEIndividual> new_indi;

            for (size_t i = 0; i < size; i++) {
                new_indi = individual->se_clone();
                new_indi->se_randomize();
                new_indi->se_calculate_fitness1();
                population.push_back(std::move(new_indi));
            }
        }

        void se_sort_population() {
            std::sort(population.begin(), population.end(),
                [] (const std::unique_ptr<SEIndividual>&ind1, const std::unique_ptr<SEIndividual>&ind2) {
                    return ind1->fitness1 < ind2->fitness1; });
        }

        void se_save_data(std::string_view filename) {
            std::vector<uint8_t> data = population[0]->se_to_vec_u8();
            std::ofstream out(std::filesystem::path(filename), std::ios::binary);

            if (!out) {
                throw SEServerIOException(fmt::format("Could not open file for writing: {}", filename).c_str());
            } else {
                out.write(reinterpret_cast<const char*>(data.data()), data.size());
            }
        }

        [[nodiscard]] virtual bool nc_is_job_done() {
            return population[0]->fitness1 <= se_config.target_fitness1;
        }

        virtual void nc_save_data() {
            se_save_data(se_config.result_filename);
        }

        [[nodiscard]] virtual std::vector<uint8_t> nc_get_new_data([[maybe_unused]] NCNodeID node_id) {
            size_t i = 0;

            if (!se_config.share_only_best) {
                i = rng.get_size_t(se_config.server_population_size);
            }

            return population[i]->se_to_vec_u8();
        }

        virtual void nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) {
            const size_t last = se_config.server_population_size - 1;

            // se_clone_internal() not needed here since all the values will be overwritten anyways!
            std::unique_ptr<SEIndividual> new_indi = population[0]->se_clone();
            // Overwrite all values from result:
            new_indi->se_from_span_u8(result);
            std::float64_t new_fitness = new_indi->fitness1;
            std::float64_t current_best_fitness = 0.0;

            if (new_fitness < population[last]->fitness1) {
                if (!se_config.allow_same_fitness) {
                    for (auto &indi: population) {
                        if (new_fitness == indi->fitness1) {
                            return;
                        }
                    }
                }

                se_logger->debug("New individual in population: fitness1: {}, actual: {}",
                    new_fitness, new_indi->se_actual_fitness());

                population[last] = std::move(new_indi);
                current_best_fitness = population[0]->fitness1;
                se_sort_population();

                if (new_fitness < current_best_fitness) {
                    new_fitness_counter++;

                    se_logger->info("New best fitness1: {}, previous: {}", new_fitness, current_best_fitness);
                    se_logger->info("From node: {}, new fitness counter: {}", node_id, new_fitness_counter);
                    se_logger->debug("Worst fitness: {}", population[last]->fitness1);

                    node_stats[node_id]++;
                    se_logger->debug("Node stats: {}", node_stats);

                    population[0]->se_new_best_individual();

                    if (se_config.save_new_fitness) {
                        se_save_data(fmt::format("{}_{}", new_fitness_counter, se_config.result_filename));
                    }

                    if (new_fitness < se_config.target_fitness1) {
                        se_logger->info("Job is done, target fitness is met.");
                        se_logger->debug("Best fitness1: {}, target fitness1: {}", population[0]->fitness1, se_config.target_fitness1);
                        se_logger->debug("Best fitness2: {}, target fitness2: {}", population[0]->fitness2, se_config.target_fitness2);
                        se_logger->debug("Actual fitness: {}", population[0]->se_actual_fitness());
                        se_logger->info("Time taken: {} sec.", sw);
                    }
                }
            }

            se_logger->flush();
        }

        // These three methods are used for the test cases:
        [[nodiscard]] size_t se_get_population_size() {
            return population.size();
        }

        [[nodiscard]] SEIndividual* se_get_individual(size_t i) {
            return population[i].get();
        }

        [[nodiscard]] SEIndividual* se_get_worst() {
            return population[se_config.server_population_size - 1].get();
        }

        [[nodiscard]] uint32_t se_get_new_fitness_counter() {
            return new_fitness_counter;
        }
};

typedef SEServerDP<SE_RNG_L64> SEServerDP_L64;
}

#endif // FILE_SE_SERVER_HPP_INCLUDED