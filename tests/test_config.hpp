/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the configuration class.

    Run only configuration tests:
    xmake run -w ./ se_test [configuration]
*/

// External includes:
#include <snitch/snitch.hpp>
#include <tao/json.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_exceptions.hpp"

using namespace secpion;

TEST_CASE("Create valid default configuration", "[configuration]" ) {
    SEConfiguration se_config;

    // Server configuration:
    REQUIRE(se_config.target_fitness1 == 0.0);
    REQUIRE(se_config.target_fitness2 == 0.0);
    REQUIRE(se_config.result_filename == "best_result.json");
    REQUIRE(se_config.save_new_fitness == true);
    REQUIRE(se_config.allow_same_fitness == false);
    REQUIRE(se_config.share_only_best == true);
    REQUIRE(se_config.server_population_size == 10);
    REQUIRE(se_config.se_server_log_file == "");
    REQUIRE(se_config.se_server_log_level == "");

    // Node configuration:
    REQUIRE(se_config.node_population_size == 10);
    REQUIRE(se_config.num_of_iterations == 1000);
    REQUIRE(se_config.num_of_mutations == 10);
    REQUIRE(se_config.accept_new_best == true);
    REQUIRE(se_config.randomize_population == false);
    REQUIRE(se_config.randomize_count == 5);
    REQUIRE(se_config.population_kind == 1);
    REQUIRE(se_config.mutation_operations == std::vector<uint8_t>{});
    REQUIRE(se_config.early_exit_sleep == 10);
    REQUIRE(se_config.se_node_log_file == "");
    REQUIRE(se_config.se_node_log_level == "");
    REQUIRE(se_config.seed_count == 10);
    REQUIRE(se_config.min_num_of_individuals == 2);
    REQUIRE(se_config.sine_base == 100.0);
    REQUIRE(se_config.sine_amplitude == 50.0);
    REQUIRE(se_config.sine_frequency == 0.01);
    REQUIRE(se_config.limit_factor == 2.0);
    REQUIRE(se_config.mutation_probability == 0.1);
    REQUIRE(se_config.crossover_probability == 0.9);
}

TEST_CASE("Test valid JSON configuration", "[configuration]" ) {
    const tao::json::value json_config = {
        {"secret_key", "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF"},
        {"target_fitness1", 2.0},
        {"target_fitness2", 4.5},
        {"result_filename", "nice_simulation.json"},
        {"save_new_fitness", false},
        {"allow_same_fitness", true},
        {"share_only_best", false},
        {"server_population_size", 15},
        {"se_server_log_file", "foo_bar"},
        {"se_server_log_level", "debug"},
        {"node_population_size", 21},
        {"num_of_iterations", 321},
        {"num_of_mutations", 7},
        {"accept_new_best", false},
        {"randomize_population", true},
        {"randomize_count", 15},
        {"population_kind", 2},
        {"mutation_operations", tao::json::value::array({2, 4, 5})},
        {"early_exit_sleep", 57},
        {"se_node_log_file", "node_foo"},
        {"se_node_log_level", "warning"},
        {"seed_count", 20},
        {"min_num_of_individuals", 8},
        {"sine_base", 77.77},
        {"sine_amplitude", 44.44},
        {"sine_frequency", 22.22},
        {"limit_factor", 33.33},
        {"mutation_probability", 0.9},
        {"crossover_probability", 0.5}
    };

    SEConfiguration se_config = se_config_from_json(json_config);

    // Server configuration:
    REQUIRE(se_config.target_fitness1 == 2.0);
    REQUIRE(se_config.target_fitness2 == 4.5);
    REQUIRE(se_config.result_filename == "nice_simulation.json");
    REQUIRE(se_config.save_new_fitness == false);
    REQUIRE(se_config.allow_same_fitness == true);
    REQUIRE(se_config.share_only_best == false);
    REQUIRE(se_config.server_population_size == 15);
    REQUIRE(se_config.se_server_log_file == "foo_bar");
    REQUIRE(se_config.se_server_log_level == "debug");

    // Node configuration:
    REQUIRE(se_config.node_population_size == 21);
    REQUIRE(se_config.num_of_iterations == 321);
    REQUIRE(se_config.num_of_mutations == 7);
    REQUIRE(se_config.accept_new_best == false);
    REQUIRE(se_config.randomize_population == true);
    REQUIRE(se_config.randomize_count == 15);
    REQUIRE(se_config.population_kind == 2);
    REQUIRE(se_config.mutation_operations == std::vector<uint8_t>{2, 4, 5});
    REQUIRE(se_config.early_exit_sleep == 57);
    REQUIRE(se_config.se_node_log_file == "node_foo");
    REQUIRE(se_config.se_node_log_level == "warning");
    REQUIRE(se_config.seed_count == 20);
    REQUIRE(se_config.min_num_of_individuals == 8);
    REQUIRE(se_config.sine_base == 77.77);
    REQUIRE(se_config.sine_amplitude == 44.44);
    REQUIRE(se_config.sine_frequency == 22.22);
    REQUIRE(se_config.limit_factor == 33.33);
    REQUIRE(se_config.mutation_probability == 0.9);
    REQUIRE(se_config.crossover_probability == 0.5);
}

/*
C++ raw string literal:
std::string multi_line = R"(
        Line 1
        Line 2
        Line 3
    )";
*/

/*
Test for exception:

TEST_CASE("Create invalid default configuration", "[configuration]" ) {
    REQUIRE_THROWS_AS(NCConfiguration("12345"), NCInvalidKeyException);
}


*/
