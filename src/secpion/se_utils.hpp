/*
    Selecppion
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines helper functions
*/

#ifndef FILE_SE_UTILS_HPP_INCLUDED
#define FILE_SE_UTILS_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <stdfloat>
#include <vector>
#include <type_traits>

// External includes:
#include <nlohmann/json.hpp>

namespace secpion {
[[nodiscard]] std::vector<uint8_t> se_json_to_vec_u8(const nlohmann::json input) {
    std::string serialized = input.dump();
    std::vector<uint8_t> result(serialized.begin(), serialized.end());

    return result;
}
}

#endif // FILE_SE_UTILS_HPP_INCLUDED
