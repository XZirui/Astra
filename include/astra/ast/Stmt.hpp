#pragma once

#include <vector>
#include "Expr.hpp"

namespace astra::ast {
    struct Stmt : ASTNode {
        explicit Stmt(const SourceRange &r) : ASTNode(r) {
        }
    };

    // statement : blockStmt
    struct BlockStmt : Stmt {
        std::vector<Stmt *> statements;

        explicit BlockStmt(const SourceRange &r) : Stmt(r) {
        }

        BlockStmt(const SourceRange &r, const std::vector<Stmt *> &statements) : Stmt(r), statements(statements) {
        }
    };

    // statement : exprStmt
    struct ExprStmt : Stmt {
        Expr *expr;

        ExprStmt(const SourceRange &r, Expr *expr) : Stmt(r), expr(expr) {
        }
    };

    // statement : ifStmt
    struct IfStmt : Stmt {
        Expr *condition;
        Stmt *thenBranch;
        Stmt *elseBranch; // optional

        IfStmt(const SourceRange &r, Expr *cond, Stmt *thenBr, Stmt *elseBr = nullptr)
            : Stmt(r), condition(cond), thenBranch(thenBr), elseBranch(elseBr) {
        }
    };

    // statement : returnStmt
    struct ReturnStmt : Stmt {
        Expr *value; // optional

        ReturnStmt(const SourceRange &r, Expr *value) : Stmt(r), value(value) {
        }
    };
}
