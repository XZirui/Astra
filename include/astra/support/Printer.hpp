#pragma once

#include <fmt/format.h>

namespace astra::support {
    class Printer {
    public:
        explicit Printer(std::ostream &out);

        void push(int amount = 2);
        void pop(int amount = 2);

        void print(std::string_view str) const;

        template<typename... Args>
        void print(fmt::format_string<Args...> fmt, Args &&... args) {
            printIndent();
            out << fmt::format(fmt, std::forward<Args>(args)...);
        }

    private:
        std::ostream &out;
        int indentLevel = 0;

        void printIndent() const;
    };
}
