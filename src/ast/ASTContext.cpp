#include "astra/ast/ASTContext.hpp"
#include "astra/ast/Node.hpp"
#include <cstring>

namespace astra::ast {
    // TODO: support For FileID management
    ASTContext::ASTContext() {
        IntType = create<BuiltinType>(BuiltinType::BuiltinTypeKind::Int);
    }

    Identifier *ASTContext::getIdentifier(std::string_view Name) {
        auto It = Identifiers.find(std::string(Name));
        if (It != Identifiers.end()) {
            return It->second;
        }

        // Not found, create a new Identifier
        auto *Storage = static_cast<char *>(
            Allocator.allocate(Name.size(), alignof(char))
        );

        std::memcpy(Storage, Name.data(), Name.size());

        std::string_view Interned(Storage, Name.size());
        auto *Id = create<Identifier>(Interned);

        Identifiers.emplace(Interned, Id);
        return Id;
    }

} // namespace astra::ast
