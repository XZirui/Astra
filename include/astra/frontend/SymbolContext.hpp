#pragma once

#include "CompilerContext.hpp"
#include "astra/sema/SymbolTable.hpp"

namespace astra::frontend {
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
            assert(CurrentScope != nullptr &&
                   "Cannot lookup symbol without an active scope");
            return CurrentScope->lookup(Name);
        }

        sema::Symbol *lookupCurrent(llvm::StringRef Name) const {
            assert(CurrentScope != nullptr &&
                   "Cannot lookup symbol without an active scope");
            return CurrentScope->lookupCurrent(Name);
        }

        bool existsCurrent(llvm::StringRef Name) const {
            assert(CurrentScope != nullptr &&
                   "Cannot lookup symbol without an active scope");
            return CurrentScope->exists(Name);
        }

        template <typename... ArgTypes> void createSymbol(ArgTypes &&...Args) {
            assert(CurrentScope != nullptr &&
                   "Cannot create symbol without an active scope");
            auto *NewSymbol = CompilerCtx.create<sema::Symbol>(
                std::forward<ArgTypes>(Args)...);
            CurrentScope->insert(NewSymbol);
        }

        class ScopeGuard {
            SymbolContext &Ctx;

        public:
            explicit ScopeGuard(SymbolContext &Ctx) : Ctx(Ctx) {
                Ctx.pushScope();
            }

            ~ScopeGuard() { Ctx.popScope(); }

            ScopeGuard(const ScopeGuard &) = delete;
            ScopeGuard &operator=(const ScopeGuard &) = delete;
        };
    };
} // namespace astra::frontend