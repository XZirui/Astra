#pragma once

#include "astra/sema/Type.hpp"
#include "astra/support/Identifier.hpp"

#include <llvm/ADT/StringMap.h>

namespace astra::sema {
    enum class SymbolKind { Variable, Function, Parameter };

    struct Symbol {
        support::Identifier *Name;
        Type                *Type;
        SymbolKind           Kind;
    };

    class SymbolTable {
        llvm::StringMap<Symbol *> CurrentScope;
        SymbolTable              *ParentScope = nullptr;

    public:
        SymbolTable() = default;
        explicit SymbolTable(SymbolTable *Parent) : ParentScope(Parent) {}

        Symbol *lookup(llvm::StringRef Name) {
            auto It = CurrentScope.find(Name);
            if (It != CurrentScope.end()) {
                return It->second;
            }
            if (ParentScope) {
                return ParentScope->lookup(Name);
            }
            return nullptr;
        }

        bool exists(llvm::StringRef Name) {
            return lookup(Name) != nullptr;
        }

        void insert(Symbol *Sym) {
            auto [It, Inserted] =
                CurrentScope.insert({Sym->Name->getName(), Sym});
            assert(Inserted && "Symbol already exists in current scope");
        }
    };

} // namespace astra::sema