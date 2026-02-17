#include "astra/frontend/CompilerContext.hpp"
#include "astra/frontend/SymbolContext.hpp"
#include "astra/frontend/TypeContext.hpp"

namespace astra::frontend {
    CompilerContext::CompilerContext()
        : TypeCtx(std::make_unique<TypeContext>(*this)),
          SymbolCtx(std::make_unique<SymbolContext>(*this)),
          DiagEngine(std::make_unique<support::DiagnosticEngine>(
              SrcMgr, FileIDMap, FileNameMap)) {}

    support::FileID CompilerContext::addSourceFile(llvm::StringRef FileName) {
        auto BufferOrError = llvm::MemoryBuffer::getFile(FileName);
        if (!BufferOrError) {
            llvm::errs() << "Error reading file: " << FileName << "\n";
            return 0;
        }

        unsigned BufferID =
            SrcMgr.AddNewSourceBuffer(std::move(*BufferOrError), llvm::SMLoc());
        static support::FileID NextID = 1;
        const support::FileID  FID = NextID;
        ++NextID;
        FileIDMap[FID] = BufferID;
        FileNameMap[FID] = FileName.str();
        return FID;
    }
} // namespace astra::frontend