#pragma once
#include <string_view>
#include <unordered_map>
#include <utility>

#include "ArenaAllocator.hpp"
#include "Type.hpp"
#include "Node.hpp"

namespace astra::ast {
    class ASTContext {
        ArenaAllocator allocator;
        std::unordered_map<std::string, Identifier *> identifiers;
        BuiltinType *intType;
        // TODO

    public:
        // TODO source Manger
        ASTContext();

        // Memory allocation for AST nodes
        template<typename T, typename... Args>
        T *create(Args &&... args) {
            void *mem = allocator.allocate(sizeof(T), alignof(T));
            return new(mem) T(std::forward<Args>(args)...);
        }

        // Identifier
        Identifier *getIdentifier(std::string_view name);

        // Builtin Types
        BuiltinType *getIntType() const { return intType; }
    };
}
