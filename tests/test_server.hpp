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

    REQUIRE(server.se_get_population_size() == 10);
    REQUIRE(server.nc_is_job_done() == false);
    REQUIRE(server.se_get_individual(0)->fitness1 < server.se_get_worst()->fitness1);
    REQUIRE(server.se_get_individual(0)->fitness1 > 0.0);

}
