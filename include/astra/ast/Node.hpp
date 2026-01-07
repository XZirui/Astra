#pragma once

#include "NodeKind.hpp"
#include "astra/support/SourceRange.hpp"

// Forward declaration
namespace astra::sema {
    class Type;
} // namespace astra::sema

namespace astra::ast {
    struct ASTNode {
        support::SourceRange Range;

        explicit ASTNode(const support::SourceRange &R) : Range(R) {}

        virtual ~ASTNode() = default;
        [[nodiscard]] virtual NodeKind getKind() const = 0;
    };
} // namespace astra::ast
