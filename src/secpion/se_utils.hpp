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
#include <tao/json.hpp>

namespace secpion {
template <typename T>
[[nodiscard]] tao::json::value se_vec_to_json(std::span<const T> input) {
    tao::json::value result = tao::json::empty_array;

    for (auto item: input) {
        if constexpr (std::is_same_v<T, std::float64_t>) {
            result.get_array().push_back(double(item));
        } else {
            result.get_array().push_back(item);
        }
    }

    return result;
}

template <typename T>
void se_json_to_vec(const tao::json::value input, std::vector<T> &result) {
    result.clear();

    for (auto item: input.get_array()) {
        if constexpr (std::is_same_v<T, std::float64_t>) {
            result.push_back(item.as<double>());
        } else {
            result.push_back(item.as<T>());
        }
    }
}

template <typename T>
[[nodiscard]] std::vector<T> se_json_to_vec(const tao::json::value input) {
    std::vector<T> result;

    se_json_to_vec(input, result);

    return result;
}

[[nodiscard]] std::vector<uint8_t> se_json_to_vec_u8(const tao::json::value input) {
    std::string serialized = tao::json::to_string(input);
    std::vector<uint8_t> result(serialized.begin(), serialized.end());

    return result;
}

[[nodiscard]] tao::json::value se_span_u8_to_json(std::span<const uint8_t> input) {
    const char* input_ptr = reinterpret_cast<const char*>(input.data());
    tao::json::value result = tao::json::from_string(input_ptr, input.size());

    return result;
}
}

#endif // FILE_SE_UTILS_HPP_INCLUDED
