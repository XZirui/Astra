#pragma once

#include <cstdint>

namespace astra::ast {
    using FileID = uint64_t;

    struct SourceLocation {
        uint64_t line = 0;
        uint64_t column = 0;
    };

    struct SourceRange {
        SourceLocation begin;
        SourceLocation end;
        FileID file;

        // TODO: support For FileID management

        SourceRange(SourceLocation begin, SourceLocation end, FileID file) : begin(begin), end(end), file(file) {
        }

        // Create SourceRange from an ANTLR4 parser rule context
        // [begin, end)
        template<typename T>
        static SourceRange rangeOf(T *ctx, FileID fileID = 0) {
            return SourceRange{
                SourceLocation{ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine() + 1},
                SourceLocation{
                    ctx->getStop()->getLine(),
                    ctx->getStop()->getCharPositionInLine() + ctx->getStop()->getText().size() + 1
                },
                fileID
            };
        }
    };
}
