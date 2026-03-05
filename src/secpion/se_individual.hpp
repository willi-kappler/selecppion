/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file defines the class for one individual.
*/

#ifndef FILE_SE_INDIVIDUAL_HPP_INCLUDED
#define FILE_SE_INDIVIDUAL_HPP_INCLUDED

// STD includes:
#include <string>
#include <cstdint>
#include <string_view>
#include <stdfloat>
#include <unordered_map>
#include <memory>
#include <limits>
#include <vector>
#include <span>

namespace secpion {
const std::float64_t SE_FLOAT_MAX = std::numeric_limits<std::float64_t>::max();

class SEIndividual {
    public:
        std::float64_t fitness1;
        std::float64_t fitness2;
        std::unordered_map<uint8_t, uint32_t> mut_op_counter;

        SEIndividual();
        virtual ~SEIndividual() = default;
        SEIndividual(SEIndividual &&) = delete;
        SEIndividual(const SEIndividual &) = delete;
        SEIndividual& operator=(const SEIndividual &) = delete;
        SEIndividual& operator=(const SEIndividual &&) = delete;

        void se_reset_counter();
        void se_mutate_internal(uint8_t);
        virtual void se_mutate(uint8_t);
        virtual void se_randomize();
        virtual void se_calculate_fitness1();
        virtual void se_calculate_fitness2();
        std::unique_ptr<SEIndividual> se_clone_internal();
        virtual std::unique_ptr<SEIndividual> se_clone();
        virtual std::vector<uint8_t> se_to_vec_u8();
        virtual void se_from_span_u8(std::span<const uint8_t>);
        virtual std::float64_t se_actual_fitness();
        virtual void se_new_best_individual();
};
}

#endif // FILE_SE_INDIVIDUAL_HPP_INCLUDED