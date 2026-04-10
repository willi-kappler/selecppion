/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the rosenbrock example

    To just build use:
    xmake build se_example_rosenbrock

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

    std::unique_ptr<RosenbrockIndividual> rosenbrock_individual =
        std::make_unique<RosenbrockIndividual>(10, -5.0, 5.0);
    global_rng.seed();
    rosenbrock_individual->se_randomize();

    make_and_run_example(argc, argv, std::move(rosenbrock_individual));
}
