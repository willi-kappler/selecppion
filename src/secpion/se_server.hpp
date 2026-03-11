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

// External includes:
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <nodcru2/nc_logger.hpp>
#include <nodcru2/nc_server.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_individual.hpp"
#include "se_random.hpp"
#include "se_exceptions.hpp"

namespace secpion {
template<typename T>
class SEServerDP: public NCServerDataProcessor {
    private:
        SEConfiguration se_config;
        std::shared_ptr<spdlog::logger> se_logger;
        std::vector<std::unique_ptr<SEIndividual>> population;
        T rng;

    public:
        SEServerDP(SEConfiguration config, std::unique_ptr<SEIndividual> individual):
            se_config(config),
            se_logger(),
            population(),
            rng(),
        {
            rng.seed();
            spdlog::drop("se_logger");
            se_logger = spdlog::stdout_logger_mt("se_logger");
            se_fill_population(std::move(individual));
            // TODO: start timer here.
        }

        void se_set_logger(std::shared_ptr<spdlog::logger> logger) {
            se_logger = logger;

            se_logger->debug("Population size: {}, target fitness 1: {}, target fitness 2: {}",
                se_config.server_population_size, se_config.target_fitness1, se_config.target_fitness2);
            se_logger->debug("Result filename: {}, save new fitness: {}",
                se_config.result_filename, se_config.save_new_fitness);
            se_logger->debug("Allow same fitness: {}, share only best: {}",
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

        void se_save_data(std::string_view filename) {
            std::vector<uint8_t> data = population[0]->se_to_vec_u8();
            std::ofstream out(std::filesystem::path(filename), std::ios::binary);

            if (!out) {
                throw SEServerIOException(fmt("Could not open file for writing: {}", filename));
            } else {
                out.write(reinterpret_cast<const char*>(data.data()), data.size());
            }
        }

        [[nodiscard]] virtual bool nc_is_job_done() {
            bool job_done = population[0]->fitness1 <= se_config.target_fitness1;

            if (job_done) {
                // TODO: stop timer here and log duration.
                se_logger->info("Job is done, target fitness is met.");
                se_logger->debug("Best fitness1: {}, target fitness1: {}", population[0]->fitness1, se_config.target_fitness1);
                se_logger->debug("Best fitness2: {}, target fitness2: {}", population[0]->fitness2, se_config.target_fitness2);
                se_logger->debug("Actual fitness: {}", population[0]->se_actual_fitness());
            }

            return job_done;
        }

        virtual void nc_save_data() {
            se_save_data(se_config.result_filename);
        }

        [[nodiscard]] virtual std::vector<uint8_t> nc_get_new_data(NCNodeID node_id) {
            size_t i = 0;

            if (!se_config.share_only_best) {
                i = rng.get_size_t(se_config.server_population_size);
            }

            return population[i]->se_to_vec_u8();
        }

        virtual void nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) {
            // TODO: implement!
        }
};
}

#endif // FILE_SE_SERVER_HPP_INCLUDED