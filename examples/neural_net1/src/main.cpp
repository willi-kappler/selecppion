/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the neuralnet1 example

    To just build use:
    xmake build se_example_neuralnet1

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {
    std::unique_ptr<NeuralNet1Individual> neuralnet1_individual =
        std::make_unique<NeuralNet1Individual>(2, 1);
    global_rng.seed();
    neuralnet1_individual->se_randomize();

    make_and_run_example(argc, argv, std::move(neuralnet1_individual));
}
