#pragma once

namespace astra::ast {

    // TODO
    enum class TypeKind {
        Builtin,
    };

    struct Type {
        TypeKind kind;
        explicit Type(TypeKind k) : kind(k) {}
    };

    struct BuiltinType : Type {
        enum class BuiltinKind {
            Int, // TODO
        };

        BuiltinKind builtinKind;

        explicit BuiltinType(BuiltinKind k) : Type(TypeKind::Builtin), builtinKind(k) {}
    };
};