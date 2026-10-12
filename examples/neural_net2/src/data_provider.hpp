/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the data provider class for the neuralnet2 example

    To just build use (from the main folder):
    ./build.sh

    Run with:
    ./run_example.sh
*/

// STD includes:
#include <algorithm>
#include <vector>
#include <utility>
#include <span>
#include <print>
#include <cmath>
#include <numbers>
#include <tuple>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

using data_set_t = std::vector<std::tuple<std::float64_t, std::float64_t, std::float64_t, std::float64_t, std::float64_t, std::float64_t, std::float64_t>>;

class DataProvider {
    private:
        const uint16_t batch_size;
        data_set_t training_set;
        data_set_t test_set;

    public:
        DataProvider():
            batch_size(20)
            training_set(),
            test_set()
        {}

        void load_data(std::filesystem::path file_path) {
            std::ifstream in_file(file_path);

            if (in_file.is_open()) {
                for (std::string line; std::getline(in_file, line); ) {
                    std::cout << line << '\n';
                }
            } else {
                throw std::runtime_error("Could not open data file!");
            }
        }
}
