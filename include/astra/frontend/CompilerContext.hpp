#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "astra/support/Allocator.hpp"
#include "astra/support/Identifier.hpp"

namespace astra::frontend {
    // Forward declaration
    class TypeContext;

    class CompilerContext {
        support::Allocator                                     Allocator{};
        std::unordered_map<std::string, support::Identifier *> Identifiers;

        std::unique_ptr<TypeContext> TypeCtx;

    public:
        CompilerContext();
        support::Identifier *getIdentifier(std::string_view Name);

        // Memory allocation
        template <typename T, typename... TypeArgs>
        T *create(TypeArgs &&...Args) {
            void *Mem = Allocator.allocate(sizeof(T), alignof(T));
            return new (Mem) T{std::forward<TypeArgs>(Args)...};
        }

        template <typename T, typename... TypeArgs>
        T *createArray(size_t Count) {
            void *Mem = Allocator.allocate(sizeof(T) * Count, alignof(T));
            return static_cast<T *>(Mem);
        }
    };
} // namespace astra::frontend
