#pragma once

#include <algorithm>
#include <cstdint>

namespace astra::support {
    using FileID = uint64_t;

    struct SourceLocation {
        uint64_t Line = 0;
        uint64_t Column = 0;
        uint64_t Offset = 0;

        auto operator<=>(const SourceLocation &Other) const {
            return Offset <=> Other.Offset;
        }
    };

    struct SourceRange {
        SourceLocation Begin;
        SourceLocation End;
        FileID         File{};

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

        SourceRange extend(const SourceRange &Other) const {
            if (Other.File != File) {
                return *this; // cannot extend across different files
            }
            return SourceRange{std::min(Begin, Other.Begin),
                               std::max(End, Other.End), File};
        }
    };
} // namespace astra::support
