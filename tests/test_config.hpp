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
#include <nodcru2/nc_config.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_exceptions.hpp"

using namespace secpion;

TEST_CASE("Create valid default configuration", "[configuration]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    SEConfiguration config2 = SEConfiguration(config1);

    // Server configuration:
    REQUIRE(config2.target_fitness1 == 0.0);
    REQUIRE(config2.target_fitness2 == 0.0);
    REQUIRE(config2.result_filename == "best_result.json");
    REQUIRE(config2.save_new_fitness == true);
    REQUIRE(config2.allow_same_fitness == false);
    REQUIRE(config2.share_only_best == true);
    REQUIRE(config2.server_population_size == 10);

    // Node configuration:
    REQUIRE(config2.node_population_size == 10);
    REQUIRE(config2.num_of_iterations == 1000);
    REQUIRE(config2.num_of_mutations == 10);
    REQUIRE(config2.accept_new_best == true);
    REQUIRE(config2.randomize_population == false);
    REQUIRE(config2.randomize_count == 10);
    REQUIRE(config2.population_kind == 1);
    REQUIRE(config2.mutation_operations == std::vector<uint8_t>{});
    REQUIRE(config2.min_num_of_individuals == 2);
    REQUIRE(config2.sine_base == 100.0);
    REQUIRE(config2.sine_amplitude == 50.0);
    REQUIRE(config2.sine_frequency == 10.0);
    REQUIRE(config2.limit_range == 10.0);
    // REQUIRE(config2. == );
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
        {"node_population_size", 21},
        {"num_of_iterations", 321},
        {"num_of_mutations", 7},
        {"accept_new_best", false},
        {"randomize_population", true},
        {"randomize_count", 15},
        {"population_kind", 2},
        {"mutation_operations", tao::json::value::array({2, 4, 5})},
        {"min_num_of_individuals", 8},
        {"sine_base", 77.77},
        {"sine_amplitude", 44.44},
        {"sine_frequency", 22.22},
        {"limit_range", 33.33}
        // { "",  }
    };

    SEConfiguration config1 = se_config_from_json(json_config);

    // Server configuration:
    REQUIRE(config1.target_fitness1 == 2.0);
    REQUIRE(config1.target_fitness2 == 4.5);
    REQUIRE(config1.result_filename == "nice_simulation.json");
    REQUIRE(config1.save_new_fitness == false);
    REQUIRE(config1.allow_same_fitness == true);
    REQUIRE(config1.share_only_best == false);
    REQUIRE(config1.server_population_size == 15);

    // Node configuration:
    REQUIRE(config1.node_population_size == 21);
    REQUIRE(config1.num_of_iterations == 321);
    REQUIRE(config1.num_of_mutations == 7);
    REQUIRE(config1.accept_new_best == false);
    REQUIRE(config1.randomize_population == true);
    REQUIRE(config1.randomize_count == 15);
    REQUIRE(config1.population_kind == 2);
    REQUIRE(config1.mutation_operations == std::vector<uint8_t>{2, 4, 5});
    REQUIRE(config1.min_num_of_individuals == 8);
    REQUIRE(config1.sine_base == 77.77);
    REQUIRE(config1.sine_amplitude == 44.44);
    REQUIRE(config1.sine_frequency == 22.22);
    REQUIRE(config1.limit_range == 33.33);
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