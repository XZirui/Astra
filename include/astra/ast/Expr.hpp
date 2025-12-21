#pragma once

#include "Node.hpp"

namespace astra::ast {
    struct Expr : ASTNode {
    };

    // primaryExpr : INTEGER_LITERAL
    struct LiteralExpr : Expr {
        uint64_t value = 0; // TODO

        explicit LiteralExpr(uint64_t value) : value(value) {}
    };

    // primaryExpr : IDENTIFIER
    struct VarExpr : Expr {
        Identifier *name;

        explicit VarExpr(Identifier *name) : name(name) {}
    };

    // unaryExpr
    struct UnaryExpr : Expr {
        enum class Op { Add, Sub }; // TODO
        Op op;
        Expr *operand;
    };

    // from disjunction to multiplication
    struct BinaryExpr : Expr {
        enum class Op { Add, Sub, Mult, Div, Mod, Eq, Neq, Lt, Gt, Le, Ge, Disj, Conj }; // TODO
        Op op;
        Expr *lhs;
        Expr *rhs;
    };
}
