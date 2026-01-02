#pragma once

#include "NodeKind.hpp"

namespace astra::ast {
    struct ASTNode;

    template<template <typename> class Ptr, typename Derived, typename RetTy = void>
    class ASTVisitorBase {
    public:
#define PTR(CLASS) typename Ptr<CLASS>::type
#define DISPATCH(NAME, CLASS)\
    return static_cast<Derived *>(this)->visit##NAME(static_cast<PTR(CLASS)>(D))

        RetTy visit(PTR(ASTNode) D) {
            switch (D->getKind()) {
#define ATTR(TYPE, NAME)
#define ATTR_LIST(TYPE, NAME)
#define ABSTRACT_NODE(NAME, BASE, ...)
#define NODE(NAME, BASE, FIELD) \
                case NodeKind::NAME: DISPATCH(NAME, NAME);
#include "AllNodes.def"
                default: throw;// TODO report errors
            }
        }

#undef NODE
#define NODE(NAME, BASE, FIELD) \
        RetTy visit##NAME(PTR(NAME) D) { DISPATCH(BASE, BASE); }
#include "AllNodes.def"
    };

#undef PTR
#undef DISPATCH
#include "macros/NodeMacrosUndef.hpp"
} // namespace astra::ast
