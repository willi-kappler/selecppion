/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the sudoku1 example

    To just build use:
    xmake build se_example_sudoku1

    Run with:
    ./run_example.sh
*/

// Local includes:
#include "secpion/se_individual.hpp"
#include "example_utils.hpp"
#include "individual.hpp"

using namespace secpion;

int main(int argc, char *argv[]) {
    std::vector<uint8_t> input = {
        0, 8, 0,   0, 9, 4,   0, 0, 0,
        2, 0, 3,   0, 0, 0,   9, 4, 0,
        0, 0, 0,   0, 0, 2,   1, 0, 3,

        0, 0, 8,   0, 0, 0,   7, 9, 0,
        9, 2, 0,   0, 0, 0,   0, 5, 6,
        0, 7, 6,   0, 0, 0,   3, 0, 0,

        0, 5, 7,   0, 0, 0,   2, 0, 1,
        3, 0, 2,   1, 0, 0,   0, 0, 0,
        0, 0, 0,   2, 6, 0,   0, 3, 0
    };
    std::unique_ptr<SudokuIndividual> sudoku1_individual =
        std::make_unique<SudokuIndividual>(input);
    sudoku1_individual->se_randomize();
    global_rng.seed();

    make_and_run_example(argc, argv, std::move(sudoku1_individual));
}
