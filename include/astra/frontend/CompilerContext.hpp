#pragma once

#include "astra/support/Diagnostic.hpp"
#include <llvm/Support/Allocator.h>
#include <llvm/Support/StringSaver.h>
#include <memory>

namespace astra::frontend {
    // Forward declaration

    class TypeContext;
    class SymbolContext;

    class CompilerContext {
        llvm::BumpPtrAllocator  Allocator{};
        llvm::UniqueStringSaver StringSaver{Allocator};

        std::unique_ptr<TypeContext>               TypeCtx;
        std::unique_ptr<SymbolContext>             SymbolCtx;
        std::unique_ptr<support::DiagnosticEngine> DiagEngine;

        llvm::SourceMgr                              SrcMgr;
        llvm::DenseMap<support::FileID, unsigned>    FileIDMap;
        llvm::DenseMap<support::FileID, std::string> FileNameMap;

    public:
        CompilerContext();

        llvm::StringRef getIdentifier(llvm::StringRef Name) {
            return StringSaver.save(Name);
        }

        llvm::BumpPtrAllocator &getAllocator() { return Allocator; }

        // Memory allocation
        template <typename T, typename... TypeArgs>
        T *create(TypeArgs &&...Args) {
            return new (Allocator) T(std::forward<TypeArgs>(Args)...);
        }

        TypeContext   &getTypeContext() const { return *TypeCtx; }
        SymbolContext &getSymbolContext() const { return *SymbolCtx; }
        support::DiagnosticEngine &getDiagEngine() const { return *DiagEngine; }
        llvm::SourceMgr           &getSrcMgr() { return SrcMgr; }

        support::FileID addSourceFile(llvm::StringRef FileName);
        unsigned        getLLVMBufferID(support::FileID FID) const {
            const auto It = FileIDMap.find(FID);
            return It != FileIDMap.end() ? It->second : 0;
        }
    };
} // namespace astra::frontend
