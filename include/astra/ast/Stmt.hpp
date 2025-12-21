#pragma once

#include <vector>
#include "Expr.hpp"

namespace astra::ast {
    struct Stmt : ASTNode {
    };

    // statement : blockStmt
    struct BlockStmt : Stmt {
        std::vector<Stmt *> statements;
    };

    // statement : exprStmt
    struct ExprStmt : Stmt {
        Expr *expr;
    };

    // statement : ifStmt
    struct IfStmt : Stmt {
        Expr *condition;
        Stmt *thenBranch;
        Stmt *elseBranch; // optional
    };

    // statement : returnStmt
    struct ReturnStmt : Stmt {
        Expr *value; // optional
    };
}
