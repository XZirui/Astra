#pragma once

#include "Node.hpp"
#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/StringRef.h>
#include <fmt/format.h>

namespace astra::ast {
    enum class Op {
        Add,
        Sub,
        Mult,
        Div,
        Mod,
        Not,
        Eq,
        Neq,
        LShift,
        RShift,
        BitAnd,
        BitXor,
        BitOr,
        BitNot,
        Lt,
        Gt,
        Le,
        Ge,
        Disj,
        Conj
    }; // TODO
    enum class FloatType { Float, Double };

#include "macros/NodeMacros.hpp" // NOLINT(*-include-order)
#include "ExprNodes.def"
#include "macros/NodeMacrosUndef.hpp"
} // namespace astra::ast

template <> struct fmt::formatter<astra::ast::Op> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::ast::Op &Operator, FormatContext &Ctx) {
        using enum astra::ast::Op;
        switch (Operator) {
        case Add:
            return fmt::format_to(Ctx.out(), "+");
        case Sub:
            return fmt::format_to(Ctx.out(), "-");
        case Mult:
            return fmt::format_to(Ctx.out(), "*");
        case Div:
            return fmt::format_to(Ctx.out(), "/");
        case Mod:
            return fmt::format_to(Ctx.out(), "%");
        case Not:
            return fmt::format_to(Ctx.out(), "!");
        case Eq:
            return fmt::format_to(Ctx.out(), "==");
        case Neq:
            return fmt::format_to(Ctx.out(), "!=");
        case LShift:
            return fmt::format_to(Ctx.out(), "<<");
        case RShift:
            return fmt::format_to(Ctx.out(), ">>");
        case BitAnd:
            return fmt::format_to(Ctx.out(), "&");
        case BitXor:
            return fmt::format_to(Ctx.out(), "^");
        case BitOr:
            return fmt::format_to(Ctx.out(), "|");
        case BitNot:
            return fmt::format_to(Ctx.out(), "~");
        case Lt:
            return fmt::format_to(Ctx.out(), "<");
        case Gt:
            return fmt::format_to(Ctx.out(), ">");
        case Le:
            return fmt::format_to(Ctx.out(), "<=");
        case Ge:
            return fmt::format_to(Ctx.out(), ">=");
        case Disj:
            return fmt::format_to(Ctx.out(), "||");
        case Conj:
            return fmt::format_to(Ctx.out(), "&&");
        default:
            return fmt::format_to(Ctx.out(), "UnknownOp");
        }
    }
};

template <> struct fmt::formatter<astra::ast::FloatType> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::ast::FloatType &Kind, FormatContext &Ctx) {
        using enum astra::ast::FloatType;
        switch (Kind) {
        case Float:
            return fmt::format_to(Ctx.out(), "float");
        case Double:
            return fmt::format_to(Ctx.out(), "double");
        default:
            return fmt::format_to(Ctx.out(), "UnknownFloatType");
        }
    }
};
