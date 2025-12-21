#pragma once

#include "SourceRange.hpp"

namespace astra::ast {
    struct Identifier {
        explicit Identifier(std::string_view s) : name(s) {}

        std::string_view getName() const { return name; }

    private:
        std::string_view name;
    };

    struct ASTNode {
        SourceRange range;
        virtual ~ASTNode() = default;
    };
}