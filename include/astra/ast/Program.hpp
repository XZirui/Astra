#pragma once

#include "Stmt.hpp"

namespace astra::ast {
    // file
    struct TopLevelObject : ASTNode {
        // TODO variants for different top-level objects
        Stmt *stmt;
    };

    struct Program : ASTNode {
        std::vector<TopLevelObject *> objects;
    };
}