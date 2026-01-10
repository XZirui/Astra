#include "astra/frontend/SymbolContext.hpp"
#include "astra/frontend/CompilerContext.hpp"

namespace astra::frontend {
    SymbolContext::SymbolContext(CompilerContext &CompilerCtx)
        : CompilerCtx(CompilerCtx) {}

    void SymbolContext::createNewScope() {
        Scopes.emplace_back(std::make_unique<sema::SymbolTable>(CurrentScope));
        CurrentScope = Scopes.back().get();
    }

    void SymbolContext::destroyCurrentScope() {
        if (!Scopes.empty()) {
            Scopes.pop_back();
            if (!Scopes.empty()) {
                CurrentScope = Scopes.back().get();
            } else {
                CurrentScope = nullptr;
            }
        }
    }

    void SymbolContext::createSymbol(support::Identifier *Identifier,
                                     sema::Type *Type, sema::SymbolKind Kind) const {
        assert(CurrentScope != nullptr && "Cannot create symbol without an active scope");
        auto *NewSymbol =
            CompilerCtx.create<sema::Symbol>(Identifier, Type, Kind);
        CurrentScope->insert(NewSymbol);
    }

} // namespace astra::frontend