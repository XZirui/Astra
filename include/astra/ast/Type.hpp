#pragma once

namespace astra::ast {

    // TODO
    enum class TypeKind {
        Builtin,
    };

    struct Type {
        TypeKind Kind;
        explicit Type(TypeKind K) : Kind(K) {}
    };

    struct BuiltinType : Type {
        enum class BuiltinTypeKind {
            Int, // TODO
        };

        BuiltinTypeKind BuiltinKind;

        explicit BuiltinType(BuiltinTypeKind K) : Type(TypeKind::Builtin), BuiltinKind(K) {}
    };
} // namespace astra::ast