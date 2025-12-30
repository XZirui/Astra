#pragma once

#include <cstddef>

namespace astra::test::mock {
    struct MockToken {
        size_t line{1};
        size_t column{0};
        size_t start{0};
        size_t stop{0};

        size_t getLine() const { return line; }
        size_t getCharPositionInLine() const { return column; }
        size_t getStartIndex() const { return start; }
        size_t getStopIndex() const { return stop; }
    };

    struct MockParserRuleContext {
        MockToken start_token;
        MockToken stop_token;

        MockToken* getStart() { return &start_token; }
        MockToken* getStop() { return &stop_token; }
    };
};