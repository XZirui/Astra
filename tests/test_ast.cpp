#include <catch2/catch_test_macros.hpp>

#include "astra/ast/Program.hpp"


// TODO
TEST_CASE("Literal stores correct value", "[ast][expr]") {
    astra::ast::LiteralExpr literal;
    literal.value = 42;
    REQUIRE(literal.value == 42);
}