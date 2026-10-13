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
#include <vector>
#include <print>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <regex>
#include <stdfloat>

// Local includes:
#include "secpion/se_random.hpp"


struct DataPoint {
    std::float64_t sl;
    std::float64_t sw;
    std::float64_t pl;
    std::float64_t pw;

    std::float_t i1;
    std::float_t i2;
    std::float_t i3;
};


class DataProvider {
    private:
        std::vector<DataPoint> training_set;
        secpion::SE_RNG_L64 local_rng;

    public:
        const uint16_t batch_size;
        std::vector<std::vector<std::float64_t>> train_input_values;
        std::vector<std::vector<std::float64_t>> train_output_values;
        std::vector<std::vector<std::float64_t>> test_input_values;
        std::vector<std::vector<std::float64_t>> test_output_values;

        DataProvider(uint16_t training_batch_size = 20):
            training_set(),
            local_rng(),
            batch_size(training_batch_size),
            train_input_values(),
            train_output_values(),
            test_input_values(),
            test_output_values()
        {
            local_rng.seed();

            for (uint16_t i = 0; i < batch_size; i++) {
                train_input_values.push_back({0.0, 0.0, 0.0, 0.0});
                train_output_values.push_back({0.0, 0.0, 0.0});
            }
        }

        void load_data(std::filesystem::path file_path) {
            std::ifstream in_file(file_path);

            if (in_file.is_open()) {
                std::string line;

                // Ignore first line:
                std::getline(in_file, line);

                const std::regex pattern(R"(^([^,]+),([^,]+),([^,]+),([^,]+),([^,]+),([^,]+)$)");
                std::smatch match;
                DataPoint dp;

                while (std::getline(in_file, line)) {
                    if (std::regex_match(line, match, pattern)) {
                        dp.sl = std::stof(match[2]);
                        dp.sw = std::stof(match[3]);
                        dp.pl = std::stof(match[4]);
                        dp.pw = std::stof(match[5]);

                        if (match[6] == "Iris-setosa") {
                            dp.i1 = 1.0;
                            dp.i2 = 0.0;
                            dp.i3 = 0.0;
                        } else if (match[6] == "Iris-versicolor") {
                            dp.i1 = 0.0;
                            dp.i2 = 1.0;
                            dp.i3 = 0.0;
                        } else {
                            dp.i1 = 0.0;
                            dp.i2 = 0.0;
                            dp.i3 = 1.0;
                        }

                        training_set.push_back(dp);
                    }
                }

                const size_t training_size = training_set.size();
                std::print("Num of data points: {}\n", training_size);
                local_rng.shuffle(training_set);

                // Split all data points into training set (80%) and test set (20%):
                const size_t test_size = training_size * 20 / 100;

                std::print("Test size: {}\n", test_size);

                test_input_values.clear();
                test_input_values.reserve(test_size);
                test_output_values.clear();
                test_output_values.reserve(test_size);

                const size_t offset = training_size - test_size;

                for (uint16_t i = 0; i < test_size; i++) {
                    test_input_values.push_back({training_set[i + offset].sl, training_set[i + offset].sw, training_set[i + offset].pl, training_set[i + offset].pw});
                    test_output_values.push_back({training_set[i + offset].i1, training_set[i + offset].i2, training_set[i + offset].i3});
                }

                training_set.resize(offset);
                training_set.shrink_to_fit();

                set_new_training_batch();
            } else {
                throw std::runtime_error("Could not open data file!");
            }
        }

        void set_new_training_batch() {
            const size_t training_size = training_set.size();
            size_t pos = 0;

            for (uint16_t i = 0; i < batch_size; i++) {
                pos = local_rng.get_size_t(training_size);
                train_input_values[i] = {training_set[pos].sl, training_set[pos].sw, training_set[pos].pl, training_set[pos].pw};
                train_output_values[i] = {training_set[pos].i1, training_set[pos].i2, training_set[pos].i3};
            }
        }
};
