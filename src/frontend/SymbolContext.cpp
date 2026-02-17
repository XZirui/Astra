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
} // namespace astra::frontend