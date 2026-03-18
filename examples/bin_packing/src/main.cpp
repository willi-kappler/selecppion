/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes the main function for the bin packing example

    To just build use:
    xmake build se_example_bin_packing

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
#include "secpion/se_population_selector.hpp"
#include "secpion/se_individual.hpp"

using namespace secpion;

SE_RNG_L64 global_rng;

class BinPackingPIndividual: public SEIndividual {
    public:
        std::vector<std::tuple<std::float64_t, std::float64_t>> positions;

        BinPackingPIndividual():
        positions()
        {
            se_randomize();
        }

        void se_mutate(uint8_t op) override {
            switch (op) {
                case 0:
                    break;
                default:
                    break;
            }
        }

        void se_randomize() override {
            global_rng.shuffle(positions);
        }

        void se_calculate_fitness1() override {
        }

        [[nodiscard]] std::unique_ptr<SEIndividual> se_clone() override {
            std::unique_ptr<BinPackingPIndividual> result = std::make_unique<BinPackingPIndividual>();

            return result;
        }

        [[nodiscard]] std::vector<uint8_t> se_to_vec_u8() override {
            tao::json::value json_numbers = tao::json::empty_array;
            tao::json::value json_tuple;

            const tao::json::value json_data;

            std::string serialized = tao::json::to_string(json_data);
            std::vector<uint8_t> result(serialized.begin(), serialized.end());

            return result;
        }

        void se_from_span_u8(std::span<const uint8_t> data) override {
            const char* data_ptr = reinterpret_cast<const char*>(data.data());
            tao::json::value restored_json = tao::json::from_string(data_ptr, data.size());
        }
};

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("se_example_bin_packing");

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

    program.add_argument("-o")
        .help("Mutation operations")
        .nargs(1, 6)
        .scan<'i', uint8_t>();

    try {
        program.parse_args(argc, argv);
    }
        catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string file_contents = se_file_to_string("bin_packing_config.json");
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

    if (program.is_used("-o")) {
        se_config.mutation_operations = program.get<std::vector<uint8_t>>("-o");
    }

    std::unique_ptr<BinPackingPIndividual> bin_packing_individual = std::make_unique<BinPackingPIndividual>();

    if (program["--server"] == true) {
        std::cout << "Server mode" << std::endl;

        std::shared_ptr<SEServerDP_L64> bin_packing_server =
            std::make_shared<SEServerDP_L64>(se_config, std::move(bin_packing_individual));
        NCServer nc_server(nc_config, bin_packing_server);
        nc_server.nc_run();
    } else {
        std::cout << "Node (client) mode" << std::endl;

        if (program.is_used("--ip")) {
            std::string server_address = program.get<std::string>("--ip");
            nc_config.server_address = server_address;
        }

        uint8_t population_type = program.get<uint8_t>("-t");
        std::shared_ptr<NCNodeDataProcessor> bin_packing_node =
            se_select_population(se_config, std::move(bin_packing_individual), population_type);
        NCNode nc_node(nc_config, bin_packing_node);
        nc_node.nc_run();
    }
}
