/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the TSP example

    To just build use:
    xmake build se_example_tsp

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {

    // For the file "city_positions2.txt"
    // a good target fitness is 8300.0.
    // This can be set in the configuration
    // file "example_config.json":
    // "target_fitness1": 8300.0
    // Best fitness:
    // 8149.752945125565

    load_data("city_positions2.txt");
    std::unique_ptr<TSPIndividual> tsp_individual = std::make_unique<TSPIndividual>();
    tsp_individual->init_positions();
    global_rng.seed();

    make_and_run_example(argc, argv, std::move(tsp_individual));
}
