#pragma once

#include <cstdint>

namespace astra::ast {
    using FileID = uint32_t;

    struct SourceLocation {
        FileID file = 0;
        uint32_t offset = 0;
    };

    struct SourceRange {
        SourceLocation begin;
        SourceLocation end;

        // TODO: add utility methods
    };
}