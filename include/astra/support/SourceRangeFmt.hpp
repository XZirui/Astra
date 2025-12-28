#pragma once

#include <fmt/core.h>
#include "SourceRange.hpp"

template<>
struct fmt::formatter<astra::support::SourceRange> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::support::SourceRange &r, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "[{}:{}-{}:{}]", r.begin.line, r.begin.column, r.end.line, r.end.column);
    }
};
