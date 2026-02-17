#pragma once

#include "SourceRange.hpp"
#include <string>
#include <llvm/ADT/DenseMap.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

namespace astra::support {
    enum class DiagnosisLevel {
        Info,
        Warning,
        Error,
        Fatal,
    };

    struct Diagnostic {
        std::string    Message;
        DiagnosisLevel Level;
        SourceRange    Range;
    };

    class DiagnosticEngine {
        const llvm::SourceMgr                     &SrcMgr;
        const llvm::DenseMap<FileID, unsigned>    &FileIDMap;
        const llvm::DenseMap<FileID, std::string> &FileNameMap;

        std::vector<Diagnostic> Diagnostics;
        size_t                  ErrorCount = 0;
        size_t                  WarningCount = 0;
        bool                    HasFatalError = false;

        void report(const SourceRange &Range, llvm::StringRef Message,
                    DiagnosisLevel Level);

        unsigned getLLVMBufferID(FileID FID) const {
            const auto It = FileIDMap.find(FID);
            return It != FileIDMap.end() ? It->second : 0;
        }

    public:
        DiagnosticEngine(const llvm::SourceMgr                     &SrcMgr,
                         const llvm::DenseMap<FileID, unsigned>    &FileIDMap,
                         const llvm::DenseMap<FileID, std::string> &FileNameMap)
            : SrcMgr(SrcMgr), FileIDMap(FileIDMap), FileNameMap(FileNameMap) {}

        void info(const SourceRange &Range, llvm::StringRef Message) {
            report(Range, Message, DiagnosisLevel::Info);
        }

        void warning(const SourceRange &Range, llvm::StringRef Message) {
            report(Range, Message, DiagnosisLevel::Warning);
        }

        void error(const SourceRange &Range, llvm::StringRef Message) {
            report(Range, Message, DiagnosisLevel::Error);
        }

        void fatal(const SourceRange &Range, llvm::StringRef Message) {
            report(Range, Message, DiagnosisLevel::Fatal);
        }

        void printDiagnostics();
        void clear() {
            Diagnostics.clear();
            ErrorCount = 0;
            WarningCount = 0;
            HasFatalError = false;
        }

        size_t getErrorCount() const { return ErrorCount; }
        size_t getWarningCount() const { return WarningCount; }
        bool   hasFatalError() const { return HasFatalError; }
        const std::vector<Diagnostic> &getDiagnostics() const {
            return Diagnostics;
        }
    };
} // namespace astra::support