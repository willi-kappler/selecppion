/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the neuralnet2 example

    To just build use (from the main folder):
    ./build.sh

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {
    std::unique_ptr<NeuralNet2Individual> neuralnet2_individual =
        std::make_unique<NeuralNet2Individual>(2, 1);
    global_rng.seed();
    neuralnet2_individual->se_randomize();

    make_and_run_example(argc, argv, std::move(neuralnet2_individual));
}
