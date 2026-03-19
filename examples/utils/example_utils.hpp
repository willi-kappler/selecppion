/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file includes some helper functions for the examples.
*/


// STD includes:
#include <stdfloat>
#include <fstream>
#include <iostream>
#include <sstream>

// External includes:
#include <argparse/argparse.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_server.hpp"
#include "secpion/se_population_selector.hpp"

using namespace secpion;

void make_and_run_example(int argc, char *argv[], std::unique_ptr<SEIndividual> individual) {
    argparse::ArgumentParser program(argv[0]);

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

    std::string file_contents = se_file_to_string("example_config.json");
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

    if (program["--server"] == true) {
        std::cout << "Server mode" << std::endl;

        std::shared_ptr<SEServerDP_L64> tsp_server =
            std::make_shared<SEServerDP_L64>(se_config, std::move(individual));
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
            se_select_population(se_config, std::move(individual), population_type);
        NCNode nc_node(nc_config, tsp_node);
        nc_node.nc_run();
    }
}
