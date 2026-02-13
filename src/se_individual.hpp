/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for on individual
*/

#ifndef FILE_SE_INDIVIDUAL_HPP_INCLUDED
#define FILE_SE_INDIVIDUAL_HPP_INCLUDED

// STD includes:
#include <string>
#include <cstdint>
#include <string_view>
#include <stdfloat>
#include <unordered_map>

namespace Selecppion {
class SEIndividual {
    private:
        std::float64_t fitness1;
        std::float64_t fitness2;
        std::unordered_map<uint8_t, uint32_t> mut_op_counter;

    public:
        SEIndividual();

        void se_reset_counter();
        void se_mutate_internal(uint8_t);
        void se_mutate(uint8_t);
        void se_randomize();
        void se_calculate_fitness1();
        void se_calculate_fitness2();
        SEIndividual se_clone_internal(); // TODO: use smart pointer
        SEIndividual se_clone(); // TODO: use smart pointer
        void se_from_server();
        void se_to_json(); // TODO: use tao json
        void se_from_json(); // TODO: use tao json
        std::float64_t se_actual_fitness();
        void se_new_best_individual();
};
}

#endif // FILE_SE_INDIVIDUAL_HPP_INCLUDED