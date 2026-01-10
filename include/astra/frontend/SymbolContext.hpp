#pragma once

#include "astra/sema/SymbolTable.hpp"

namespace astra::frontend {
    class CompilerContext;

    class SymbolContext {
        CompilerContext                                &CompilerCtx;
        sema::SymbolTable                              *CurrentScope{nullptr};
        std::vector<std::unique_ptr<sema::SymbolTable>> Scopes;

        void createNewScope();
        void destroyCurrentScope();

    public:
        explicit SymbolContext(CompilerContext &CompilerCtx);

        void pushScope() { createNewScope(); }
        void popScope() { destroyCurrentScope(); }

        sema::Symbol *lookup(llvm::StringRef Name) const {
            assert(CurrentScope != nullptr && "Cannot lookup symbol without an active scope");
            return CurrentScope->lookup(Name);
        }

        void createSymbol(support::Identifier *Identifier, sema::Type *Type,
                          sema::SymbolKind Kind) const;
    };
} // namespace astra::frontend