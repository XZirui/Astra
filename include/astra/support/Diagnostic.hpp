#pragma once

#include "SourceRange.hpp"
#include <string>

namespace astra::support {
    enum class DiagnosisLevel { Info, Warning, Error, Fatal };

    struct Diagnostic {
        std::string        Message;
        DiagnosisLevel    Level;
        SourceRange       Range;
    };

    class DiagnosticEngine {
    public:
        void report(const Diagnostic &Diag, DiagnosisLevel Level);
    };
} // namespace astra::support