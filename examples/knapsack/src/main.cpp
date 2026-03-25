/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the bin knapsack example

    To just build use:
    xmake build se_example_bin_knapsack

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {
    std::unique_ptr<KnapSackIndividual> knapsack_individual =
        std::make_unique<KnapSackIndividual>(std::vector<std::float64_t>{
        27.98, 33.36, 25.27, 24.44, 31.32, 35.00, 29.84, 20.32,
        21.34, 26.59, 30.18, 25.71, 39.13, 24.73, 20.38, 23.90,
        24.44, 21.37, 25.01, 18.32, 10.83, 15.77, 19.99, 13.25,
        16.54, 17.90, 39.87, 35.21}, 100.0);
    knapsack_individual->se_randomize();
    global_rng.seed();

    make_and_run_example(argc, argv, std::move(knapsack_individual));
}
