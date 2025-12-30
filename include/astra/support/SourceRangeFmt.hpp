#pragma once

#include "SourceRange.hpp"
#include <fmt/core.h>

template<>
struct fmt::formatter<astra::support::SourceRange> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::support::SourceRange &R, FormatContext &Ctx) {
        return fmt::format_to(Ctx.out(), "[{}:{}-{}:{}]", R.Begin.Line, R.Begin.Column, R.End.Line, R.End.Column);
    }
};
