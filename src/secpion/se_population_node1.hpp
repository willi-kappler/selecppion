/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for the population type 1.
*/

#ifndef FILE_SE_POPULATION_NODE1_HPP_INCLUDED
#define FILE_SE_POPULATION_NODE1_HPP_INCLUDED

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
class SEPopulationNode1: public NCNodeDataProcessor {
    private:
        SEPopulation population;
        uint32_t offset;

    public:
        SEPopulationNode1(SEConfiguration, std::unique_ptr<SEIndividual>);
        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override;

        SEIndividual* get_individual(size_t);

        void se_set_logger(std::shared_ptr<spdlog::logger>);
        void se_set_loglevel(spdlog::level::level_enum);
        void se_set_file_logger(std::string_view);
};
}

#endif // FILE_SE_POPULATION_NODE1_HPP_INCLUDED