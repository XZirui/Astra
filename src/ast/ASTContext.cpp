#include "astra/ast/ASTContext.hpp"

#include <string>
#include <cstring>

namespace astra::ast {
    ASTContext::ASTContext(): identifiers() {
        intType = create<BuiltinType>(BuiltinType::BuiltinKind::Int);
    }

    Identifier *ASTContext::getIdentifier(std::string_view name) {
        auto it = identifiers.find(std::string(name));
        if (it != identifiers.end()) {
            return it->second;
        }

        // Not found, create a new Identifier
        auto storage = static_cast<char *>(
            allocator.allocate(name.size(), alignof(char))
        );

        std::memcpy(storage, name.data(), name.size());

        std::string_view interned(storage, name.size());
        auto *identifier = create<Identifier>(interned);

        identifiers.emplace(interned, identifier);
        return identifier;
    }

}
