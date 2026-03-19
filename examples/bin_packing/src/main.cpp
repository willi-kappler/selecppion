/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the TSP example

    To just build use:
    xmake build se_example_bin_packing

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {
    std::unique_ptr<BinPackingIndividual> bin_packing_individual = std::make_unique<BinPackingIndividual>();

    make_and_run_example(argc, argv, std::move(bin_packing_individual));
}
