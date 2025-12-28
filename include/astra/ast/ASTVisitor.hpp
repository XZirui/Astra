#pragma once

#include "Program.hpp"

#include "ASTVisitorBase.hpp"

namespace astra::ast {
    template<typename Derived, typename RetTy = void>
    class ASTVisitor : public ASTVisitorBase<std::add_pointer, Derived, RetTy> {};

    template <typename T> struct make_const_ptr {
        using type = std::add_pointer_t<std::add_const_t<T>>;
    };

    template<typename Derived>
    class ASTConstVisitor : public ASTVisitorBase<make_const_ptr, Derived> {};
}
