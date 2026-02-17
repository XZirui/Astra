#pragma once

#include "astra/sema/Type.hpp"
#include "astra/support/SourceRange.hpp"
#include <llvm/ADT/StringMap.h>

namespace astra::sema {
    enum class SymbolKind { Variable, Function, Parameter, Class };

    struct Symbol {
        llvm::StringRef      Name;
        Type                *Type;
        SymbolKind           Kind;
        support::SourceRange DefinitionRange;
        bool                 IsConst = false;
        bool                 IsComplete = false;
    };

    class SymbolTable {
        llvm::StringMap<Symbol *> CurrentScope;
        SymbolTable              *ParentScope = nullptr;

    public:
        SymbolTable() = default;
        explicit SymbolTable(SymbolTable *Parent) : ParentScope(Parent) {}

        Symbol *lookup(llvm::StringRef Name) {
            auto *Target = lookupCurrent(Name);
            return Target ? Target
                          : (ParentScope ? ParentScope->lookup(Name) : nullptr);
        }

        Symbol *lookupCurrent(llvm::StringRef Name) {
            auto It = CurrentScope.find(Name);
            return It != CurrentScope.end() ? It->second : nullptr;
        }

        bool exists(llvm::StringRef Name) { return lookup(Name) != nullptr; }

        void insert(Symbol *Sym) {
            auto [It, Inserted] = CurrentScope.insert({Sym->Name, Sym});
            assert(Inserted && "Symbol already exists in current scope");
        }
    };

} // namespace astra::sema