#include "astra/support/Diagnostic.hpp"
#include <fmt/core.h>

template <> struct fmt::formatter<astra::support::DiagnosisLevel> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::support::DiagnosisLevel &Level,
                FormatContext                        &Ctx) {
        switch (Level) {
            using enum astra::support::DiagnosisLevel;
        case Info:
            return fmt::format_to(Ctx.out(), "info");
        case Warning:
            return fmt::format_to(Ctx.out(), "warning");
        case Error:
            return fmt::format_to(Ctx.out(), "error");
        case Fatal:
            return fmt::format_to(Ctx.out(), "fatal error");
        }
    }
};

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
            auto BeginLine = Begin.Line;
            auto EndLine = End.Line;

            fmt::print("{}:{}:{}: {}: {}\n", FileNameMap.at(File), Begin.Line,
                       Begin.Column, Level, Message);
            if (BeginLine == EndLine) {
                fmt::print("{} | {}\n", BeginLine,
                           getLineContent(File, Begin.Line).str());
                fmt::print("  | {:>{}}^{:~<{}}\n", "", Begin.Column - 1, "",
                           End.Column - Begin.Column);
            } else {
                auto LineContent = getLineContent(File, BeginLine);
                fmt::print("{} | {}\n", BeginLine, LineContent.str());
                fmt::print("  | {:>{}}^{:~<{}}\n", "", Begin.Column - 1, "",
                           LineContent.size() - Begin.Column + 1);
                for (size_t Line = BeginLine + 1; Line < EndLine; ++Line) {
                    LineContent = getLineContent(File, Line);
                    fmt::print("{} | {}\n", Line, LineContent.str());
                    fmt::print("  | {:~<{}}\n", "", LineContent.size());
                }
                LineContent = getLineContent(File, EndLine);
                fmt::print("{} | {}\n", EndLine, LineContent.str());
                fmt::print("  | {:~<{}}\n", "", End.Column);
            }
        }
        clear();
    }
} // namespace astra::support