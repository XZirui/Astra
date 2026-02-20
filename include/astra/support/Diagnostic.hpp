#pragma once

#include "SourceRange.hpp"
#include <string>
#include <llvm/ADT/DenseMap.h>
#include <llvm/Support/SourceMgr.h>

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
        const llvm::SourceMgr                      &SrcMgr;
        const llvm::DenseMap<FileID, unsigned>     &FileIDMap;
        const llvm::DenseMap<FileID, std::string>  &FileNameMap;
        llvm::DenseMap<FileID, std::vector<size_t>> LineOffsetCache;

        std::vector<Diagnostic> Diagnostics;
        size_t                  ErrorCount = 0;
        size_t                  WarningCount = 0;
        bool                    HasFatalError = false;

        void report(const SourceRange &Range, llvm::StringRef Message,
                    DiagnosisLevel Level);

        llvm::StringRef getLineContent(FileID File, size_t Line) {
            unsigned BufferID = getLLVMBufferID(File);
            if (BufferID == 0) {
                return {};
            }
            auto *Buffer = SrcMgr.getMemoryBuffer(BufferID);
            auto  FileContent = Buffer->getBuffer();
            auto &LineIndex = getLineOffset(File);
            if (Line == 0 || Line > LineIndex.size()) {
                return {};
            }

            auto LineStart = LineIndex[Line - 1];
            auto LineEnd = LineIndex[Line] - 1; // Exclude the newline character
            return FileContent.slice(LineStart, LineEnd);
        }

        unsigned getLLVMBufferID(FileID FID) const {
            const auto It = FileIDMap.find(FID);
            return It != FileIDMap.end() ? It->second : 0;
        }

        std::vector<size_t> &getLineOffset(FileID File) {
            auto It = LineOffsetCache.find(File);
            if (It != LineOffsetCache.end()) {
                return It->second;
            }

            unsigned BufferID = getLLVMBufferID(File);
            auto    *Buffer = SrcMgr.getMemoryBuffer(BufferID);

            llvm::StringRef     FileContent = Buffer->getBuffer();
            std::vector<size_t> Lines = {0};
            for (size_t I = 0; I < FileContent.size(); ++I) {
                if (FileContent[I] == '\n') {
                    Lines.emplace_back(I + 1);
                }
            }
            Lines.emplace_back(FileContent.size());
            return LineOffsetCache[File] = std::move(Lines);
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