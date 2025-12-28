#include "astra/support/Printer.hpp"

namespace astra::support {
    Printer::Printer(std::ostream &out) : out(out) {
    }

    void Printer::push(int amount) { indentLevel += amount; }

    void Printer::pop(int amount) { indentLevel -= amount; }

    void Printer::print(std::string_view str) const {
        printIndent();
        out << str;
    }

    void Printer::printIndent() const {
        for (int indent = 0; indent < indentLevel; ++indent) {
            out << " ";
        }
    }
}
