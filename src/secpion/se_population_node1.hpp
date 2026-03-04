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
//#include <string>
//#include <unordered_map>
//#include <string_view>
//#include <stdfloat>

// External includes:
#include <nodcru2/nc_node.hpp>
//#include <tao/json.hpp>
//#include <spdlog/spdlog.h>
//#include <nodcru2/nc_logger.hpp>

// Local includes:
#include "se_config.hpp"
#include "se_population.hpp"
//#include "se_individual.hpp"
//#include "se_random.hpp"

namespace secpion {
class SEPopulationNode1: public NCNodeDataProcessor {
    private:
        SEPopulation population;
        uint32_t offset;

    public:
        SEPopulationNode1(SEConfiguration, std::unique_ptr<SEIndividual>);
        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override;
};
}

#endif // FILE_SE_POPULATION_NODE1_HPP_INCLUDED