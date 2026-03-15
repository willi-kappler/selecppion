/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the server class.

    Run only server tests:
    xmake run -w ./ se_test [server]
*/

// External includes:
#include <snitch/snitch.hpp>
#include <nodcru2/nc_config.hpp>

// Local includes:
#include "secpion/se_config.hpp"
#include "secpion/se_exceptions.hpp"
#include "secpion/se_server.hpp"
#include "utils.hpp"

using namespace secpion;

template<typename T>
SEServerDP<T> make_server() {
    return SEServerDP<T>(make_config(), std::make_unique<TestIndividual1>());
}

TEST_CASE("Test server, basic configuration", "[server]" ) {
    SEServerDP<TestRNG> server = make_server<TestRNG>();
    server.se_set_loglevel(spdlog::level::level_enum::debug);

    // Default server population size is 10:
    REQUIRE(server.se_get_population_size() == 10);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_individual(0)->fitness1 < server.se_get_worst()->fitness1);
    REQUIRE(server.se_get_individual(0)->fitness1 > 0.0);

}

TEST_CASE("Test get new data 1", "[server]" ) {
    SEConfiguration se_config = make_config();
    se_config.share_only_best = true;
    SEServerDP<TestRNG> server = SEServerDP<TestRNG>(se_config, std::make_unique<TestIndividual1>());
    server.se_set_loglevel(spdlog::level::level_enum::debug);
    NCNodeID node_id;
    std::vector<uint8_t> data = server.nc_get_new_data(node_id);
    TestIndividual1 individual;
    individual.se_from_span_u8(data);

    REQUIRE(individual.fitness1 == server.se_get_individual(0)->fitness1);
}

TEST_CASE("Test get new data 2", "[server]" ) {
    SEConfiguration se_config = make_config();
    se_config.share_only_best = false;
    SEServerDP<TestRNG> server = SEServerDP<TestRNG>(se_config, std::make_unique<TestIndividual1>());
    server.se_set_loglevel(spdlog::level::level_enum::debug);
    NCNodeID node_id;
    std::vector<uint8_t> data;
    TestIndividual1 individual;

    uint8_t fitness_counter = 0;

    for (uint8_t i = 0; i < 100; i++) {
        data = server.nc_get_new_data(node_id);
        individual.se_from_span_u8(data);

        if (individual.fitness1 != server.se_get_individual(0)->fitness1) {
            fitness_counter++;
        }
    }

    REQUIRE(fitness_counter > 10);
}

TEST_CASE("Test process result 1", "[server]" ) {
    SEConfiguration se_config = make_config();
    se_config.allow_same_fitness = false;
    se_config.save_new_fitness = false;
    SEServerDP<TestRNG> server = SEServerDP<TestRNG>(se_config, std::make_unique<TestIndividual1>());
    server.se_set_loglevel(spdlog::level::level_enum::debug);
    NCNodeID node_id;
    TestIndividual1 individual;
    individual.fitness1 = server.se_get_individual(0)->fitness1 * 1.1537;
    std::float64_t worst_fitness = server.se_get_worst()->fitness1;

    REQUIRE(individual.fitness1 > 0.0);
    REQUIRE(server.se_get_new_fitness_counter() == 0);

    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 0);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_worst()->fitness1 < worst_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 < individual.fitness1);
}

TEST_CASE("Test process result 2", "[server]" ) {
    SEConfiguration se_config = make_config();
    se_config.allow_same_fitness = false;
    se_config.save_new_fitness = false;
    SEServerDP<TestRNG> server = SEServerDP<TestRNG>(se_config, std::make_unique<TestIndividual1>());
    server.se_set_loglevel(spdlog::level::level_enum::debug);
    NCNodeID node_id;
    TestIndividual1 individual;
    std::float64_t best_fitness = server.se_get_individual(0)->fitness1;
    std::float64_t worst_fitness = server.se_get_worst()->fitness1;
    individual.fitness1 = best_fitness * 0.91573;

    REQUIRE(individual.fitness1 > 0.0);
    REQUIRE(server.se_get_new_fitness_counter() == 0);

    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 1);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_worst()->fitness1 < worst_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 < best_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 == individual.fitness1);
    REQUIRE(server.se_get_individual(1)->fitness1 == best_fitness);
}

TEST_CASE("Test process result 3", "[server]" ) {
    SEConfiguration se_config = make_config();
    se_config.allow_same_fitness = false;
    se_config.save_new_fitness = false;
    SEServerDP<TestRNG> server = SEServerDP<TestRNG>(se_config, std::make_unique<TestIndividual1>());
    server.se_set_loglevel(spdlog::level::level_enum::debug);
    NCNodeID node_id;
    TestIndividual1 individual;
    std::float64_t best_fitness = server.se_get_individual(0)->fitness1;
    individual.fitness1 = best_fitness * 0.9;

    REQUIRE(individual.fitness1 > 0.0);
    REQUIRE(server.se_get_new_fitness_counter() == 0);

    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 1);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_individual(0)->fitness1 < best_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 == individual.fitness1);
    REQUIRE(server.se_get_individual(1)->fitness1 == best_fitness);

    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 1);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_individual(0)->fitness1 < best_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 == individual.fitness1);
    REQUIRE(server.se_get_individual(1)->fitness1 == best_fitness);
}

TEST_CASE("Test process result 4", "[server]" ) {
    SEConfiguration se_config = make_config();
    se_config.allow_same_fitness = true;
    se_config.save_new_fitness = false;
    SEServerDP<TestRNG> server = SEServerDP<TestRNG>(se_config, std::make_unique<TestIndividual1>());
    server.se_set_loglevel(spdlog::level::level_enum::debug);
    NCNodeID node_id;
    TestIndividual1 individual;
    std::float64_t best_fitness = server.se_get_individual(0)->fitness1;
    individual.fitness1 = best_fitness * 0.9;

    REQUIRE(individual.fitness1 > 0.0);
    REQUIRE(server.se_get_new_fitness_counter() == 0);

    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 1);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_individual(0)->fitness1 < best_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 == individual.fitness1);
    REQUIRE(server.se_get_individual(1)->fitness1 == best_fitness);

    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 1);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_individual(0)->fitness1 < best_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 == individual.fitness1);
    REQUIRE(server.se_get_individual(1)->fitness1 == individual.fitness1);
    REQUIRE(server.se_get_individual(2)->fitness1 == best_fitness);
}

TEST_CASE("Test process result 5", "[server]" ) {
    SEConfiguration se_config = make_config();
    se_config.allow_same_fitness = false;
    se_config.save_new_fitness = false;
    SEServerDP<TestRNG> server = SEServerDP<TestRNG>(se_config, std::make_unique<TestIndividual1>());
    server.se_set_loglevel(spdlog::level::level_enum::debug);
    NCNodeID node_id;
    TestIndividual1 individual;
    std::float64_t best_fitness = server.se_get_individual(0)->fitness1;
    individual.fitness1 = best_fitness * 0.9;

    REQUIRE(individual.fitness1 > 0.0);
    REQUIRE(server.se_get_new_fitness_counter() == 0);

    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 1);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_individual(0)->fitness1 < best_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 == individual.fitness1);
    REQUIRE(server.se_get_individual(1)->fitness1 == best_fitness);

    individual.fitness1 = 0.0;
    server.nc_process_result(node_id, individual.se_to_vec_u8());

    REQUIRE(server.se_get_new_fitness_counter() == 2);
    REQUIRE(server.nc_is_job_done() == true);
    REQUIRE(server.se_get_individual(0)->fitness1 < best_fitness);
    REQUIRE(server.se_get_individual(0)->fitness1 == 0.0);
    REQUIRE(server.se_get_individual(1)->fitness1 > 0.0);
    REQUIRE(server.se_get_individual(2)->fitness1 == best_fitness);
}
