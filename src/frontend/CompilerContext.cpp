#include "astra/frontend/CompilerContext.hpp"

#include "astra/frontend/TypeContext.hpp"

#include <cstring>

namespace astra::frontend {
    CompilerContext::CompilerContext()
        : TypeCtx(std::make_unique<TypeContext>(*this)) {}

    support::Identifier *CompilerContext::getIdentifier(std::string_view Name) {
        auto It = Identifiers.find(std::string(Name));
        if (It != Identifiers.end()) {
            return It->second;
        }

        // Not found, create a new Identifier
        auto *Storage =
            static_cast<char *>(Allocator.allocate(Name.size(), alignof(char)));

        std::memcpy(Storage, Name.data(), Name.size());
        std::string_view Interned(Storage, Name.size());

        auto *Id = create<support::Identifier>(Interned);
        Identifiers.emplace(Interned, Id);
        return Id;
    }
} // namespace astra::frontend