/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for one individual.
*/

// Local includes:
#include "se_individual.hpp"
#include "se_exceptions.hpp"

namespace secpion {

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
    // Can be implemented but is not needed.
}

[[nodiscard]] std::unique_ptr<SEIndividual> SEIndividual::se_clone_internal() {
    std::unique_ptr<SEIndividual> clone = se_clone();
    clone->mut_op_counter = mut_op_counter;
    clone->fitness1 = fitness1;
    clone->fitness2 = fitness2;

    return clone;
}

[[nodiscard]] std::unique_ptr<SEIndividual> SEIndividual::se_clone() {
    throw SEIndividualException("se_clone() must be implemented!");
}

[[nodiscard]] std::vector<uint8_t> SEIndividual::se_to_vec_u8() {
    throw SEIndividualException("se_to_vec_u8() must be implemented!");
}

void SEIndividual::se_from_span_u8(std::span<const uint8_t>) {
    throw SEIndividualException("se_from_span_u8() must be implemented!");
}

[[nodiscard]] std::float64_t SEIndividual::se_actual_fitness() {
    // Can be changed if the fitness needs to be adjusted.
    return fitness1;
}

void SEIndividual::se_new_best_individual() {
    // Can be implemented but is not needed.
    // This method will be called whenever there is a new
    // best individual on the server or in a population.
}

void SEIndividual::se_reseed_rng([[maybe_unused]] size_t index) {
    // Can be implemented but is not needed.
    // This will be called whenever seed_count (SEConfiguration) is reached inside
    // the population seed_counter (SEPopulation).
    // index is the current index of this individual inside the population.
}

[[nodiscard]] std::unique_ptr<SEIndividual> SEIndividual::se_crossover([[maybe_unused]] const SEIndividual* const individual) {
    // Must only be implemented if using population type 11.
    throw SEIndividualException("se_crossover() must be implemented for population type 11!");
}
}
