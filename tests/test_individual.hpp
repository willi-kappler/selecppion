/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the individual class.

    Run only individual tests:
    xmake run -w ./ se_test [individual]
*/

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "secpion/se_individual.hpp"
#include "secpion/se_exceptions.hpp"
#include "utils.hpp"

using namespace secpion;

TEST_CASE("Test mutate internal with base class", "[individual]" ) {
    SEIndividual individual;
    individual.fitness1 = 1.1;
    individual.fitness2 = 2.2;
    REQUIRE(individual.mut_op_counter.size() == 0);

    // Add some operations:
    REQUIRE_THROWS_AS(individual.se_mutate_internal(11), SEIndividualException);
    REQUIRE(individual.fitness1 == 1.1);
    REQUIRE(individual.fitness2 == 2.2);
    REQUIRE(individual.mut_op_counter.size() == 1);
    REQUIRE(individual.mut_op_counter[11] == 1);

    individual.fitness1 = 2.1;
    individual.fitness2 = 5.2;
    REQUIRE_THROWS_AS(individual.se_mutate_internal(17), SEIndividualException);
    REQUIRE(individual.fitness1 == 2.1);
    REQUIRE(individual.fitness2 == 5.2);
    REQUIRE(individual.mut_op_counter.size() == 2);
    REQUIRE(individual.mut_op_counter[11] == 1);
    REQUIRE(individual.mut_op_counter[17] == 1);

    individual.fitness1 = 10.7;
    individual.fitness2 = 52.8;
    REQUIRE_THROWS_AS(individual.se_mutate_internal(23), SEIndividualException);
    REQUIRE(individual.fitness1 == 10.7);
    REQUIRE(individual.fitness2 == 52.8);
    REQUIRE(individual.mut_op_counter.size() == 3);
    REQUIRE(individual.mut_op_counter[11] == 1);
    REQUIRE(individual.mut_op_counter[17] == 1);
    REQUIRE(individual.mut_op_counter[23] == 1);

    individual.fitness1 = 9.2;
    individual.fitness2 = 4.3;
    REQUIRE_THROWS_AS(individual.se_mutate_internal(11), SEIndividualException);
    REQUIRE(individual.fitness1 == 9.2);
    REQUIRE(individual.fitness2 == 4.3);
    REQUIRE(individual.mut_op_counter.size() == 3);
    REQUIRE(individual.mut_op_counter[11] == 2);
    REQUIRE(individual.mut_op_counter[17] == 1);
    REQUIRE(individual.mut_op_counter[23] == 1);

    individual.fitness1 = 1.9;
    individual.fitness2 = 0.5;
    individual.se_reset_counter();
    REQUIRE(individual.fitness1 == 1.9);
    REQUIRE(individual.fitness2 == 0.5);
    REQUIRE(individual.mut_op_counter.size() == 0);
}

TEST_CASE("Check required methods", "[individual]" ) {
    SEIndividual individual;

    REQUIRE_THROWS_AS(individual.se_mutate(1), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_randomize(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_calculate_fitness1(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_clone(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_to_vec_u8(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_from_span_u8({}), SEIndividualException);
}

TEST_CASE("Test clone with derived class", "[individual]" ) {
    TestIndividual1 individual;
    individual.fitness1 = 12.5;
    individual.fitness2 = 156.0;
    individual.mut_op_counter[15] = 2;
    individual.mut_op_counter[21] = 7;

    REQUIRE(individual.mut_op_counter.size() == 2);
    REQUIRE(individual.mutate_called == 0);
    REQUIRE(individual.clone_called == 0);

    // Test call to se_clone_internal():
    std::unique_ptr<SEIndividual> individual2 = individual.se_clone_internal();
    std::unique_ptr<TestIndividual1> individual3(static_cast<TestIndividual1*>(individual2.release()));

    REQUIRE(individual.mut_op_counter.size() == 2);
    REQUIRE(individual.mutate_called == 0);
    REQUIRE(individual.clone_called == 1);

    REQUIRE(individual3->fitness1 == 12.5);
    REQUIRE(individual3->fitness2 == 156.0);
    REQUIRE(individual3->mut_op_counter.size() == 2);
    REQUIRE(individual3->mut_op_counter[15] == 2);
    REQUIRE(individual3->mut_op_counter[21] == 7);
    REQUIRE(individual3->mutate_called == 0);
    REQUIRE(individual3->clone_called == 0);
}

TEST_CASE("Test mutate internal with derived class", "[individual]" ) {
    TestIndividual1 individual;
    individual.fitness1 = 59.3;
    individual.fitness2 = 31.2;

    REQUIRE(individual.mut_op_counter.size() == 0);
    REQUIRE(individual.mutate_called == 0);
    REQUIRE(individual.clone_called == 0);

    individual.se_mutate_internal(3);
    individual.se_mutate_internal(15);
    individual.se_mutate_internal(22);
    individual.se_mutate_internal(22);

    REQUIRE(individual.fitness1 == 59.3);
    REQUIRE(individual.fitness2 == 31.2);
    REQUIRE(individual.mut_op_counter.size() == 3);
    REQUIRE(individual.mut_op_counter[3] == 1);
    REQUIRE(individual.mut_op_counter[15] == 1);
    REQUIRE(individual.mut_op_counter[22] == 2);
    REQUIRE(individual.mutate_called == 4);
    REQUIRE(individual.clone_called == 0);
}
