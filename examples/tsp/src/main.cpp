/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the TSP example

    To just build use:
    xmake build se_example_tsp

    Run with:
    ./run_example.sh
*/

// STD includes:
#include <vector>
#include <tuple>
#include <stdfloat>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

// External includes:
#include <argparse/argparse.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_server.hpp"
//#include "secpion/se_population_node1.hpp"
#include "secpion/se_population_selector.hpp"
#include "secpion/se_individual.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

class TSPIndividual: public SEIndividual {
    public:
        std::vector<std::tuple<std::float64_t, std::float64_t>> positions;

        TSPIndividual():
        positions()
        {
            se_randomize();
        }

        void load_data(std::string_view filename) {
            std::string city_positions = se_file_to_string(filename);
            std::istringstream sstream(city_positions);
            std::float64_t x, y;

            positions.clear();

            while (sstream >> x >> y) {
                positions.push_back({x, y});
            }
        }

        size_t get_one_index() {
            return global_rng.get_size_t(positions.size());
        }

        std::tuple<size_t, size_t> get_two_indices() {
            size_t size = positions.size();
            size_t i1 = global_rng.get_size_t(size);
            size_t i2 = global_rng.get_size_t(size);

            while (i1 == i2) {
                i2 = global_rng.get_size_t(size);
            }

            return {i1, i2};
        }

        void reverse() {
            auto [i1, i2] = get_two_indices();

            if (i1 > i2) {
                std::swap(i1, i2);
            }

            std::reverse(positions.begin() + i1, positions.begin() + i2);
        }

        void just_swap() {
            const auto [i1, i2] = get_two_indices();
            std::swap(positions[i1], positions[i2]);
        }

        void shift_left() {
            auto [i1, i2] = get_two_indices();

            if (i1 > i2) {
                std::swap(i1, i2);
            }

            std::rotate(positions.begin() + i1, positions.begin() + i1 + 1, positions.begin() + i2);
        }

        void shift_right() {
            auto [i1, i2] = get_two_indices();

            if (i1 > i2) {
                std::swap(i1, i2);
            }

            std::rotate(positions.begin() + i1, positions.begin() + i2 - 1, positions.begin() + i2);
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    reverse();
                    break;
                case 1:
                    just_swap();
                    break;
                case 2:
                    shift_left();
                    break;
                default:
                    shift_right();
                    break;
            }
        }

        void se_randomize() override {
            global_rng.shuffle(positions);
        }

        void se_calculate_fitness1() override {
            fitness1 = 0.0;

            auto [x0, y0] = *(positions.end() - 1);

            for (auto [x1, y1]: positions) {
                fitness1 += hypot(x0 - x1, y0 - y1);
                x0 = x1;
                y0 = y1;
            }
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<TSPIndividual> result = std::make_unique<TSPIndividual>();
            result->positions = positions;

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_numbers = tao::json::empty_array;
            tao::json::value json_tuple;

            for (auto [x, y]: positions) {
                json_tuple = tao::json::empty_array;
                json_tuple.push_back(double(x));
                json_tuple.push_back(double(y));
                json_numbers.get_array().push_back(json_tuple);
            }

            const tao::json::value json_data = {
                {"fitness1", double(fitness1)},
                {"positions", json_numbers},
            };

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());

            fitness1 = restored_json["fitness1"].as<double>();

            const auto& arr1 = restored_json["positions"].get_array();

            for (size_t i = 0; i < positions.size(); i++) {
                //positions[i] = arr1[i].as<std::tuple<double, double>>();
                auto arr2 = arr1[i].get_array();
                std::float64_t x = arr2[0].as<double>();
                std::float64_t y = arr2[1].as<double>();
                positions[i] = {x, y};
            }
        }
};

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("se_example_tsp");

    program.add_argument("--server")
        .help("Set server mode")
        .flag();

    program.add_argument("--ip")
        .help("Set the ip address for the server")
        .default_value(std::string(""));

    program.add_argument("-r")
        .help("Randomize population")
        .flag();

    program.add_argument("-m")
        .help("Number of mutations")
        .scan<'i', uint32_t>();

    program.add_argument("-i")
        .help("Number of iterations")
        .scan<'i', uint32_t>();

    program.add_argument("-p")
        .help("Population size")
        .scan<'i', uint32_t>();

    program.add_argument("-t")
        .help("Population type")
        .default_value(static_cast<uint8_t>(1))
        .scan<'i', uint8_t>();

    try {
        program.parse_args(argc, argv);
    }
        catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string file_contents = se_file_to_string("tsp_config.json");
    NCConfiguration nc_config = nc_config_from_string(file_contents);
    SEConfiguration se_config = se_config_from_string(file_contents);

    if (program["-r"] == true) {
        se_config.randomize_population = true;
    }

    if (program.is_used("-m")) {
        se_config.num_of_mutations = program.get<uint32_t>("-m");
    }

    if (program.is_used("-i")) {
        se_config.num_of_iterations = program.get<uint32_t>("-i");
    }

    if (program.is_used("-p")) {
        se_config.node_population_size = program.get<uint32_t>("-p");
    }

    // For the file "city_positions2.txt"
    // a good target fitness is 8300.0.
    // This can be set in the configuration
    // file "tsp_config.json":
    // "target_fitness1": 8300.0

    std::unique_ptr<TSPIndividual> tsp_individual = std::make_unique<TSPIndividual>();
    tsp_individual->load_data("city_positions2.txt");

    if (program["--server"] == true) {
        std::cout << "Server mode" << std::endl;

        std::shared_ptr<SEServerDP_L64> tsp_server =
            std::make_shared<SEServerDP_L64>(se_config, std::move(tsp_individual));
        NCServer nc_server(nc_config, tsp_server);
        nc_server.nc_run();
    } else {
        std::cout << "Node (client) mode" << std::endl;

        if (program.is_used("--ip")) {
            std::string server_address = program.get<std::string>("--ip");
            nc_config.server_address = server_address;
        }

        uint8_t population_type = program.get<uint8_t>("-t");
        std::shared_ptr<NCNodeDataProcessor> tsp_node =
            se_select_population(se_config, std::move(tsp_individual), population_type);
        NCNode nc_node(nc_config, tsp_node);
        nc_node.nc_run();
    }
}
