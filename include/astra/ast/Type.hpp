#pragma once

#include <string_view>

#include "Node.hpp"

namespace astra::ast {
    // TODO
    enum class TypeKind {
        Builtin,
        UserDefined,
        TypeRef,
    };

    struct Type {
        TypeKind Kind;

        explicit Type(TypeKind K) : Kind(K) {
        }

        virtual ~Type() = default;

        virtual std::string_view ToString() const = 0;
    };

    struct BuiltinType : Type {
        enum class BuiltinTypeKind {
            Int, // TODO
        };

        BuiltinTypeKind BuiltinKind;

        explicit BuiltinType(BuiltinTypeKind K) : Type(TypeKind::Builtin), BuiltinKind(K) {
        }

        std::string_view ToString() const override {
            switch (BuiltinKind) {
                case BuiltinTypeKind::Int:
                    return "int";
                default:
                    return "unknown_builtin_type";
            }
        }
    };

    // TODO UserDefinedType

    // Unresolved type reference (raw name)
    struct TypeRef : Type {
        Identifier *Name;

        explicit TypeRef(Identifier *Name) : Type(TypeKind::TypeRef), Name(Name) {
        }

        std::string_view ToString() const override {
            return Name->GetName();
        }
    };
} // namespace astra::ast
