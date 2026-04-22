/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the queens example

    To just build use:
    xmake build se_example_queens

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {
    std::unique_ptr<QueensIndividual> queens_individual =
        std::make_unique<QueensIndividual>();
    queens_individual->se_randomize();
    global_rng.seed();

    make_and_run_example(argc, argv, std::move(queens_individual));
}
