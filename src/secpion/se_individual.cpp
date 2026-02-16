/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for on individual
*/

// STD includes:
#include <limits>

// Local includes:
#include "se_individual.hpp"
#include "se_exceptions.hpp"

namespace secpion {
const std::float64_t SE_FLOAT_MAX = std::numeric_limits<std::float64_t>::max();

SEIndividual::SEIndividual():
    fitness1(SE_FLOAT_MAX),
    fitness2(SE_FLOAT_MAX),
    mut_op_counter()
{};

void SEIndividual::se_reset_counter() {
    mut_op_counter.clear();
}

void SEIndividual::se_mutate_internal(uint8_t mut_op) {
    mut_op_counter[mut_op]++;
    se_mutate(mut_op);
}

void SEIndividual::se_mutate([[maybe_unused]] uint8_t mut_op) {
    throw SEIndividualException("se_mutate() must be implemented!");
}

void SEIndividual::se_randomize() {
    throw SEIndividualException("se_randomize() must be implemented!");
}

void SEIndividual::se_calculate_fitness1() {
    throw SEIndividualException("se_calculate_fitness1() must be implemented!");
}

void SEIndividual::se_calculate_fitness2() {
}

}