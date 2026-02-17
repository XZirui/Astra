#include "astra/support/Diagnostic.hpp"

namespace astra::support {
    void DiagnosticEngine::report(const SourceRange &Range,
                                  llvm::StringRef    Message,
                                  DiagnosisLevel     Level) {
        Diagnostics.emplace_back(Message.str(), Level, Range);
        switch (Level) {
        case DiagnosisLevel::Warning:
            ++WarningCount;
            break;
        case DiagnosisLevel::Fatal:
            HasFatalError = true;
            [[fallthrough]];
        case DiagnosisLevel::Error:
            ++ErrorCount;
            break;
        default:
            break;
        }
    }

    void DiagnosticEngine::printDiagnostics() {
        for (auto &[Message, Level, Range] : Diagnostics) {
            const auto &[Begin, End, File] = Range;

            unsigned BufferID = getLLVMBufferID(File);
            if (BufferID == 0) {
                return;
            }
            auto *Buffer = SrcMgr.getMemoryBuffer(BufferID);
            auto *BufferStart = Buffer->getBufferStart();

            auto LocStart =
                llvm::SMLoc::getFromPointer(BufferStart + Begin.Offset);
            auto LocEnd = llvm::SMLoc::getFromPointer(BufferStart + End.Offset);

            llvm::SourceMgr::DiagKind Kind;
            switch (Level) {
            case DiagnosisLevel::Info:
                Kind = llvm::SourceMgr::DK_Note;
                break;
            case DiagnosisLevel::Warning:
                Kind = llvm::SourceMgr::DK_Warning;
                break;
            case DiagnosisLevel::Error:
                Kind = llvm::SourceMgr::DK_Error;
                break;
            default:
                Kind = llvm::SourceMgr::DK_Error;
                break;
            }

            SrcMgr.PrintMessage(llvm::errs(), LocStart, Kind, Message,
                {{LocStart, LocEnd}});
        }
        clear();
    }
} // namespace astra::support