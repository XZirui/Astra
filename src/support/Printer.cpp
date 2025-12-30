#include "astra/support/Printer.hpp"

namespace astra::support {
    Printer::Printer(std::ostream &Out) : Out(Out) {
    }

    void Printer::push(int Amount) { IndentLevel += Amount; }

    void Printer::pop(int Amount) { IndentLevel -= Amount; }

    void Printer::print(std::string_view Str) const {
        printIndent();
        Out << Str;
    }

    void Printer::printIndent() const {
        for (int Indent = 0; Indent < IndentLevel; ++Indent) {
            Out << " ";
        }
    }
} // namespace astra::support
