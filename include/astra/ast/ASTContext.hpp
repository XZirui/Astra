#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "ArenaAllocator.hpp"
#include "Type.hpp"

namespace astra::ast {
    struct Identifier;

    class ASTContext {
        ArenaAllocator Allocator;
        std::unordered_map<std::string, Identifier *> Identifiers;
        BuiltinType *IntType;
        // TODO

    public:
        // TODO source Manger
        ASTContext();

        // Memory allocation for AST nodes
        template<typename T, typename... Args>
        T *create(Args &&... Arguments) {
            void *Mem = Allocator.allocate(sizeof(T), alignof(T));
            return new(Mem) T{std::forward<Args>(Arguments)...};
        }

        // Identifier
        Identifier *getIdentifier(std::string_view Name);

        // Builtin Types
        BuiltinType *getIntType() const { return IntType; }
    };
} // namespace astra::ast
