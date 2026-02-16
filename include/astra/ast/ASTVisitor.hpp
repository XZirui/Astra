#pragma once

#include "Program.hpp"
#include "ASTVisitorBase.hpp"

namespace astra::ast {
    template <typename T> struct MakeConstPtr {
        using type = std::add_pointer_t<std::add_const_t<T>>;
    };

    template <typename Derived, typename RetTy = void>
    class ASTVisitor : public ASTVisitorBase<std::add_pointer, Derived, RetTy> {
    };

    template <typename Derived, typename RetTy = void>
    class ASTConstVisitor
        : public ASTVisitorBase<MakeConstPtr, Derived, RetTy> {};

    template <typename Derived, typename RetTy = void>
    class DeclVisitor
        : public DeclVisitorBase<std::add_pointer, Derived, RetTy> {};

    template <typename Derived, typename RetTy = void>
    class DeclConstVisitor
        : public DeclVisitorBase<MakeConstPtr, Derived, RetTy> {};

    template <typename Derived, typename RetTy = void>
    class ExprVisitor
        : public ExprVisitorBase<std::add_pointer, Derived, RetTy> {};

    template <typename Derived, typename RetTy = void>
    class ExprConstVisitor
        : public ExprVisitorBase<MakeConstPtr, Derived, RetTy> {};
} // namespace astra::ast
