/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the rastrigin example

    To just build use:
    xmake build se_example_rastrigin

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {
    // The exact best solution should be 0.
    // Sinve we have rounding errors it is set to 0.0001 in the
    // configuration file.

    std::unique_ptr<RastriginIndividual> rastrigin_individual =
        std::make_unique<RastriginIndividual>(10, -5.0, 5.0);
    global_rng.seed();
    rastrigin_individual->se_randomize();

    make_and_run_example(argc, argv, std::move(rastrigin_individual));
}
