/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the utility functions.

    Run only configuration tests:
    builddir/selecppion_tests [utils]
*/

// STD includes:
#include <format>

// External includes:
#include <snitch/snitch.hpp>
#include <nlohmann/json.hpp>

// Local includes:
#include "secpion/se_utils.hpp"

using namespace secpion;

TEST_CASE("Test utility function se_json_to_vec_u8()", "[utils]" ) {
    nlohmann::json json_data;
    std::vector<uint8_t> result;
    std::string vec_as_string;

    json_data = {};
    result = se_json_to_vec_u8(json_data);
    vec_as_string.assign_range(result);
    REQUIRE(vec_as_string == "null");

    json_data = {67};
    result = se_json_to_vec_u8(json_data);
    vec_as_string.assign_range(result);
    REQUIRE(vec_as_string == "[67]");

    json_data = {false};
    result = se_json_to_vec_u8(json_data);
    vec_as_string.assign_range(result);
    REQUIRE(vec_as_string == "[false]");

    json_data = {5, 7, 10};
    result = se_json_to_vec_u8(json_data);
    vec_as_string.assign_range(result);
    REQUIRE(vec_as_string == "[5,7,10]");

    json_data = {{"health", 5.3}};
    result = se_json_to_vec_u8(json_data);
    vec_as_string.assign_range(result);
    // INFO("result: ", std::format("{}", result));
    // INFO("vec_as_string: ", vec_as_string);
    REQUIRE(vec_as_string == "{\"health\":5.3}");
}
