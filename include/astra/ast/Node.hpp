#pragma once

#include <string_view>

#include "astra/support/SourceRange.hpp"
#include "NodeKind.hpp"

namespace astra::ast {
    struct Identifier {
        explicit Identifier(std::string_view s) : name(s) {
        }

        [[nodiscard]] std::string_view getName() const { return name; }

    private:
        std::string_view name;
    };

    struct ASTNode {
        support::SourceRange range;

        explicit ASTNode(const support::SourceRange &r) : range(r) {
        }

        virtual ~ASTNode() = default;
        [[nodiscard]] virtual NodeKind getKind() const = 0;
    };
}
