#pragma once

#include "NodeKind.hpp"

namespace astra::ast {
    struct ASTNode;

    template <template <typename> class Ptr, typename Derived,
              typename RetTy = void>
    class ASTVisitorBase {
    public:
#define PTR(CLASS) typename Ptr<CLASS>::type
#define DISPATCH(NAME, CLASS)                                                  \
    return static_cast<Derived *>(this)->visit##NAME(static_cast<PTR(CLASS)>(D))

#include "macros/NodeMacrosEmpty.hpp"

#undef NODE
#define NODE(NAME, BASE, FIELDS)                                               \
    case NodeKind::NAME:                                                       \
        DISPATCH(NAME, NAME);

        RetTy visit(PTR(ASTNode) D) {
            switch (D->getKind()) {
#include "AllNodes.def"

            default:
                throw; // TODO report errors
            }
        }

#undef NODE
#define NODE(NAME, BASE, ...)                                                  \
    RetTy visit##NAME(PTR(NAME) D) { DISPATCH(BASE, BASE); }
#include "AllNodes.def"
    };

    template <template <typename> class Ptr, typename Derived,
              typename RetTy = void>
    class DeclVisitorBase {
    public:
#undef NODE
#define NODE(NAME, BASE, FIELDS)                                               \
    case NodeKind::NAME:                                                       \
        DISPATCH(NAME, NAME);

        RetTy visit(PTR(ASTNode) D) {
            switch (D->getKind()) {
#include "DeclNodes.def"

#include "TypeRefNodes.def"

#include "ProgramNodes.def"

            default:
                throw; // TODO report errors
            }
        }

#undef NODE
#define NODE(NAME, BASE, ...)                                                  \
    RetTy visit##NAME(PTR(NAME) D) { DISPATCH(BASE, BASE); }
#include "DeclNodes.def"

#include "TypeRefNodes.def"

#include "ProgramNodes.def"
    };

    template <template <typename> class Ptr, typename Derived,
              typename RetTy = void>
    class ExprVisitorBase {
    public:
#undef NODE
#define NODE(NAME, BASE, FIELDS)                                               \
    case NodeKind::NAME:                                                       \
        DISPATCH(NAME, NAME); /* no break: DISPATCH always returns */

        RetTy visit(PTR(ASTNode) D) {
            switch (D->getKind()) {
#include "ExprNodes.def"

            default:
                throw; // TODO report errors
            }
        }

#undef NODE
#define NODE(NAME, BASE, ...)                                                  \
    RetTy visit##NAME(PTR(NAME) D) { DISPATCH(BASE, BASE); }
#include "ExprNodes.def"
    };

#undef PTR
#undef DISPATCH
#include "macros/NodeMacrosUndef.hpp"

} // namespace astra::ast
