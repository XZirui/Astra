#pragma once

#include <string_view>

#include "astra/support/SourceRange.hpp"
#include "NodeKind.hpp"

namespace astra::ast {
    struct Identifier {
        explicit Identifier(std::string_view S) : Name(S) {
        }

        [[nodiscard]] std::string_view GetName() const { return Name; }

    private:
        std::string_view Name;
    };

    struct ASTNode {
        support::SourceRange Range;

        explicit ASTNode(const support::SourceRange &R) : Range(R) {
        }

        virtual ~ASTNode() = default;
        [[nodiscard]] virtual NodeKind getKind() const = 0;
    };
} // namespace astra::ast
