/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the configuration options
*/

// STD includes:
#include <fstream>
#include <iostream>

// External includes:
#include <tao/json.hpp>

// Local includes:
#include "se_config.hpp"

namespace Selecppion {

SEConfiguration::SEConfiguration() {}

[[nodiscard]] SEConfiguration se_config_from_string(std::string_view config_as_string) {
    const tao::json::value json_config = tao::json::from_string(config_as_string);

    SEConfiguration config;

    return config;
}

[[nodiscard]] SEConfiguration se_config_from_file(std::filesystem::path file_path) {
    std::ifstream in_file {file_path};

    if (in_file.is_open()) {
        std::string file_contents {std::istreambuf_iterator<char>(in_file), std::istreambuf_iterator<char>()};
        return se_config_from_string(file_contents);
    } else {
        // TODO: Throw exception
    }
}

}
