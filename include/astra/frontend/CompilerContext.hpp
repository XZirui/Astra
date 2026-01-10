#pragma once

#include "astra/support/Identifier.hpp"
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/Allocator.h>
#include <llvm/Support/StringSaver.h>
#include <memory>

namespace astra::frontend {
    // Forward declaration

    class TypeContext;
    class SymbolContext;

    class CompilerContext {
        llvm::BumpPtrAllocator                 Allocator{};
        llvm::UniqueStringSaver                StringSaver{Allocator};
        llvm::StringMap<support::Identifier *> IdentifierMap{};

        std::unique_ptr<TypeContext> TypeCtx;
        std::unique_ptr<SymbolContext> SymbolCtx;

    public:
        CompilerContext();
        support::Identifier *getIdentifier(llvm::StringRef Name);

        llvm::BumpPtrAllocator &getAllocator() { return Allocator; }

        // Memory allocation
        template <typename T, typename... TypeArgs>
        T *create(TypeArgs &&...Args) {
            return new (Allocator) T(std::forward<TypeArgs>(Args)...);
        }
    };
} // namespace astra::frontend
