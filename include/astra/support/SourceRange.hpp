#pragma once

#include <cstdint>

namespace astra::support {
    using FileID = uint64_t;

    struct SourceLocation {
        uint64_t Line = 0;
        uint64_t Column = 0;
        uint64_t Offset = 0;
    };

    struct SourceRange {
        SourceLocation Begin;
        SourceLocation End;
        FileID         File{};

        // TODO: support For FileID management

        // Create SourceRange from an ANTLR4 parser rule context
        // [begin, end)
        template <typename T>
        static SourceRange rangeOf(T *Ctx, FileID FileId = 0) {
            auto *Start = Ctx->getStart();
            auto *Stop = Ctx->getStop();

            return SourceRange{
                SourceLocation{Start->getLine(),
                               Start->getCharPositionInLine() + 1,
                               static_cast<uint64_t>(Start->getStartIndex())},
                SourceLocation{Stop->getLine(),
                               Stop->getCharPositionInLine() +
                                   Stop->getText().size() + 1,
                               static_cast<uint64_t>(Stop->getStopIndex() + 1)},
                FileId};
        }
    };
} // namespace astra::support
