#pragma once

#include <cstdint>

namespace astra::support {
    using FileID = uint64_t;

    struct SourceLocation {
        uint64_t Line = 0;
        uint64_t Column = 0;
    };

    struct SourceRange {
        SourceLocation Begin;
        SourceLocation End;
        FileID File{};

        // TODO: support For FileID management

        // Create SourceRange from an ANTLR4 parser rule context
        // [begin, end)
        template<typename T>
        static SourceRange rangeOf(T *Ctx, FileID FileId = 0) {
            return SourceRange{
                SourceLocation{Ctx->getStart()->getLine(), Ctx->getStart()->getCharPositionInLine() + 1},
                SourceLocation{
                    Ctx->getStop()->getLine(),
                    Ctx->getStop()->getCharPositionInLine() + Ctx->getStop()->getText().size() + 1
                },
                FileId
            };
        }
    };
}
