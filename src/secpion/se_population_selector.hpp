/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for one population
*/

#ifndef FILE_SE_POPULATION_SELECTOR_HPP_INCLUDED
#define FILE_SE_POPULATION_SELECTOR_HPP_INCLUDED

// STD includes:


// Local includes:
#include "se_population_node1.hpp"
#include "se_population_node2.hpp"
#include "se_population_node3.hpp"
#include "se_population_node4.hpp"
#include "se_population_node5.hpp"
#include "se_population_node6.hpp"
#include "se_population_node7.hpp"
#include "se_population_node8.hpp"
#include "se_population_node9.hpp"
#include "se_population_node10.hpp"
#include "se_population_node11.hpp"

namespace secpion {
std::shared_ptr<NCNodeDataProcessor> se_select_population(SEConfiguration se_configuration, std::unique_ptr<SEIndividual> individual, uint8_t population_type) {
    std::shared_ptr<NCNodeDataProcessor> result;

    switch (population_type) {
        case 1:
            result = std::make_shared<SEPop1_L64>(se_configuration, std::move(individual));
            break;
        case 2:
            result = std::make_shared<SEPop2_L64>(se_configuration, std::move(individual));
            break;
        case 3:
            result = std::make_shared<SEPop3_L64>(se_configuration, std::move(individual));
            break;
        case 4:
            result = std::make_shared<SEPop4_L64>(se_configuration, std::move(individual));
            break;
        case 5:
            result = std::make_shared<SEPop5_L64>(se_configuration, std::move(individual));
            break;
        case 6:
            result = std::make_shared<SEPop6_L64>(se_configuration, std::move(individual));
            break;
        case 7:
            result = std::make_shared<SEPop7_L64>(se_configuration, std::move(individual));
            break;
        case 8:
            result = std::make_shared<SEPop8_L64>(se_configuration, std::move(individual));
            break;
        case 9:
            result = std::make_shared<SEPop9_L64>(se_configuration, std::move(individual));
            break;
        case 10:
            result = std::make_shared<SEPop10_L64>(se_configuration, std::move(individual));
            break;
        case 11:
            result = std::make_shared<SEPop11_L64>(se_configuration, std::move(individual));
            break;
        default:
            throw SEPopulationException("Unknown population type: ");
            break;
    }

    return result;
}
}

#endif // FILE_SE_POPULATION_SELECTOR_HPP_INCLUDED
