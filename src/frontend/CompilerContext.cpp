#include "astra/frontend/CompilerContext.hpp"

#include "astra/frontend/TypeContext.hpp"
#include "astra/frontend/SymbolContext.hpp"

namespace astra::frontend {
    CompilerContext::CompilerContext()
        : TypeCtx(std::make_unique<TypeContext>(*this)),
          SymbolCtx(std::make_unique<SymbolContext>(*this)) {}

    support::Identifier *CompilerContext::getIdentifier(llvm::StringRef Name) {
        auto *It = IdentifierMap[Name];
        if (It) {
            return It;
        }

        auto SavedName = StringSaver.save(Name);
        return IdentifierMap[Name] = create<support::Identifier>(SavedName);
    }
} // namespace astra::frontend