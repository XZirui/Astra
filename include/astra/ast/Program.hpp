#pragma once

#include "Stmt.hpp"

namespace astra::ast {
    // file
    struct TopLevelObject : ASTNode {
        // TODO variants for different top-level objects
        Stmt *stmt;

        TopLevelObject(const SourceRange &r, Stmt *stmt) : ASTNode(r), stmt(stmt) {
        }
    };

    struct Program : ASTNode {
        std::vector<TopLevelObject *> objects;

        explicit Program(const SourceRange &r) : ASTNode(r) {
        }

        Program(const SourceRange &r, const std::vector<TopLevelObject *> &objects) : ASTNode(r), objects(objects) {
        }
    };
}
