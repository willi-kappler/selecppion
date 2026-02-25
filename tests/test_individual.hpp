/*
    Selecppion
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/selecppion

    This file contains the tests for the individual class.

    Run only configuration tests:
    xmake run -w ./ se_test [individual]
*/

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "secpion/se_individual.hpp"
#include "secpion/se_exceptions.hpp"

using namespace secpion;

class TestIndividual: public SEIndividual {
    public:
        uint8_t mutate_called;
        uint8_t clone_called;

        TestIndividual();
        void se_mutate(uint8_t) override;
        std::unique_ptr<SEIndividual> se_clone() override;
};

TestIndividual::TestIndividual():
    SEIndividual(),
    mutate_called(0),
    clone_called(0)
{}

void TestIndividual::se_mutate([[maybe_unused]] uint8_t mut_op) {
    mutate_called++;
}

std::unique_ptr<SEIndividual> TestIndividual::se_clone() {
    clone_called++;
    return std::make_unique<TestIndividual>();
}

TEST_CASE("Create empty individual", "[individual]" ) {
    SEIndividual individual;

    REQUIRE(individual.fitness1 == SE_FLOAT_MAX);
    REQUIRE(individual.fitness2 == SE_FLOAT_MAX);
    REQUIRE(individual.mut_op_counter.size() == 0);

    // Add some operations:
    REQUIRE_THROWS_AS(individual.se_mutate_internal(11), SEIndividualException);
    REQUIRE(individual.mut_op_counter.size() == 1);
    REQUIRE(individual.mut_op_counter[11] == 1);

    REQUIRE_THROWS_AS(individual.se_mutate_internal(17), SEIndividualException);
    REQUIRE(individual.mut_op_counter.size() == 2);
    REQUIRE(individual.mut_op_counter[11] == 1);
    REQUIRE(individual.mut_op_counter[17] == 1);

    REQUIRE_THROWS_AS(individual.se_mutate_internal(23), SEIndividualException);
    REQUIRE(individual.mut_op_counter.size() == 3);
    REQUIRE(individual.mut_op_counter[11] == 1);
    REQUIRE(individual.mut_op_counter[17] == 1);
    REQUIRE(individual.mut_op_counter[23] == 1);

    REQUIRE_THROWS_AS(individual.se_mutate_internal(11), SEIndividualException);
    REQUIRE(individual.mut_op_counter.size() == 3);
    REQUIRE(individual.mut_op_counter[11] == 2);
    REQUIRE(individual.mut_op_counter[17] == 1);
    REQUIRE(individual.mut_op_counter[23] == 1);

    individual.se_reset_counter();
    REQUIRE(individual.mut_op_counter.size() == 0);
}

TEST_CASE("Check required methods", "[individual]" ) {
    SEIndividual individual;

    REQUIRE_THROWS_AS(individual.se_mutate(1), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_randomize(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_calculate_fitness1(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_clone(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_from_server(std::make_unique<SEIndividual>()), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_to_json(), SEIndividualException);
    REQUIRE_THROWS_AS(individual.se_from_json({}), SEIndividualException);
}

TEST_CASE("Test clone with derived class", "[individual]" ) {
    TestIndividual individual;
    individual.fitness1 = 12.5;
    individual.fitness2 = 156.0;
    individual.mut_op_counter[15] = 2;
    individual.mut_op_counter[21] = 7;

    REQUIRE(individual.mut_op_counter.size() == 2);
    REQUIRE(individual.mutate_called == 0);
    REQUIRE(individual.clone_called == 0);

    // Test call to se_clone_internal():
    std::unique_ptr<SEIndividual> individual2 = individual.se_clone_internal();
    std::unique_ptr<TestIndividual> individual3(static_cast<TestIndividual*>(individual2.release()));

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

TEST_CASE("Test clone with derived class", "[individual]" ) {
    TestIndividual individual;
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
