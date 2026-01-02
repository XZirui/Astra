#pragma once

#include <cstddef>

namespace astra::test::mock {
    struct MockToken {
        size_t Line{1};
        size_t Column{0};
        size_t Start{0};
        size_t Stop{0};
        std::string Text;

        size_t getLine() const { return Line; }
        size_t getCharPositionInLine() const { return Column; }
        size_t getStartIndex() const { return Start; }
        size_t getStopIndex() const { return Stop; }
        std::string getText() const { return Text; }
    };

    struct MockParserRuleContext {
        MockToken StartToken;
        MockToken StopToken;

        MockToken* getStart() { return &StartToken; }
        MockToken* getStop() { return &StopToken; }
    };
};