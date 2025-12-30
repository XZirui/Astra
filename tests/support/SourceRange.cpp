#include <catch2/catch_test_macros.hpp>

#include "astra/support/SourceRange.hpp"

TEST_CASE("SourceLocation - basic properties", "[support][SourceRange]") {
    astra::support::SourceLocation Loc;
    STATIC_REQUIRE(std::is_integral_v<decltype(Loc.Line)>);
    STATIC_REQUIRE(std::is_integral_v<decltype(Loc.Column)>);

    REQUIRE(Loc.Line == 0);
    REQUIRE(Loc.Column == 0);

    Loc.Line = 10;
    Loc.Column = 5;

    REQUIRE(Loc.Line == 10);
    REQUIRE(Loc.Column == 5);
}

TEST_CASE("SourceLocation - construction", "[support][SourceRange]") {
    astra::support::SourceLocation Loc{3, 7};

    REQUIRE(Loc.Line == 3);
    REQUIRE(Loc.Column == 7);
}

TEST_CASE("SourceRange - basic properties", "[support][SourceRange]") {
    astra::support::SourceRange Range;
    STATIC_REQUIRE(std::is_same_v<decltype(Range.Begin), astra::support::SourceLocation>);
    STATIC_REQUIRE(std::is_same_v<decltype(Range.End), astra::support::SourceLocation>);
    STATIC_REQUIRE(std::is_integral_v<decltype(Range.File)>);

    REQUIRE(Range.Begin.Line == 0);
    REQUIRE(Range.Begin.Column == 0);
    REQUIRE(Range.End.Line == 0);
    REQUIRE(Range.End.Column == 0);
    REQUIRE(Range.File == 0);
}

TEST_CASE("SourceRange - construction", "[support][SourceRange]") {
    astra::support::SourceRange Range{
                {1, 5},  // begin
                {3, 10}, // end
                42       // file
            };
    REQUIRE(Range.Begin.Line == 1);
    REQUIRE(Range.Begin.Column == 5);
    REQUIRE(Range.End.Line == 3);
    REQUIRE(Range.End.Column == 10);
    REQUIRE(Range.File == 42);
}

TEST_CASE("SourceRange - rangeOf", "[support][SourceRange]") {

}