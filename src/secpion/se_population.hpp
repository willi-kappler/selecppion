/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for one population
*/

#ifndef FILE_SE_POPULATION_HPP_INCLUDED
#define FILE_SE_POPULATION_HPP_INCLUDED

// STD includes:
//#include <string>
//#include <unordered_map>
#include <cstdint>
#include <string_view>
#include <stdfloat>
#include <memory>

// External includes:
#include <tao/json.hpp>
#include <spdlog/spdlog.h>
#include <nodcru2/nc_logger.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_individual.hpp"
#include "se_random.hpp"

namespace secpion {
class SEPopulation {
    public:
        SEConfiguration se_config;
        std::shared_ptr<spdlog::logger> se_logger;

        std::vector<std::unique_ptr<SEIndividual>> population;
        size_t best_index;
        size_t worst_index;
        size_t mut_op_index;
        bool minimum_found;
        SERandomGenerator<SEAlgorithmLehmer64> rng;

        SEPopulation(SEConfiguration);

        void se_set_logger(std::shared_ptr<spdlog::logger>);
        void se_set_loglevel(spdlog::level::level_enum);
        void se_set_file_logger(std::string_view);

        void se_find_worst_individual();
        void se_find_best_and_worst_individual();
        void se_sort_population();
        void se_random_population();
        void se_randomize_or_accept_best(std::unique_ptr<SEIndividual>);
        void se_shuffle_mutation_operations();
        void se_randomize_worst();
        void se_replace_best(std::unique_ptr<SEIndividual>);
        void se_replace_worst(std::unique_ptr<SEIndividual>);
        void se_clone_best_to_worst();
        std::unique_ptr<SEIndividual> se_get_best();
        std::float64_t se_get_best_fitness();
        std::float64_t se_get_worst_fitness();
        uint8_t se_get_mut_op();
        void se_check_limit(std::unique_ptr<SEIndividual>, std::float64_t, size_t);
        void se_early_exit(uint64_t);
        void se_calculate_fitness2();
        void se_log_statistics();

};
}

#endif // FILE_SE_POPULATION_HPP_INCLUDED