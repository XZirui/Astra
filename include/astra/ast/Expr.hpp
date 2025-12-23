#pragma once

#include "Node.hpp"

namespace astra::ast {
    struct Expr : ASTNode {
        explicit Expr(const SourceRange &r) : ASTNode(r) {
        }
    };

    // primaryExpr : INTEGER_LITERAL
    struct LiteralExpr : Expr {
        uint64_t value = 0; // TODO

        explicit LiteralExpr(const SourceRange &r, uint64_t value) : Expr(r), value(value) {
        }
    };

    // primaryExpr : IDENTIFIER
    struct VarExpr : Expr {
        Identifier *name;

        explicit VarExpr(const SourceRange &r, Identifier *name) : Expr(r), name(name) {
        }
    };

    // unaryExpr
    struct UnaryExpr : Expr {
        enum class Op { Add, Sub }; // TODO
        Op op;
        Expr *operand;

        UnaryExpr(const SourceRange &r, Op op, Expr *operand) : Expr(r), op(op), operand(operand) {
        }
    };

    // from disjunction to multiplication
    struct BinaryExpr : Expr {
        enum class Op { Add, Sub, Mult, Div, Mod, Eq, Neq, Lt, Gt, Le, Ge, Disj, Conj }; // TODO
        Op op;
        Expr *lhs;
        Expr *rhs;

        BinaryExpr(const SourceRange &r, Op op, Expr *lhs, Expr *rhs) : Expr(r), op(op), lhs(lhs), rhs(rhs) {
        }
    };
}
