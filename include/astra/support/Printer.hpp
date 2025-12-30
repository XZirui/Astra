#pragma once

#include <fmt/format.h>

namespace astra::support {
    class Printer {
    public:
        explicit Printer(std::ostream &Out);

        void push(int Amount = 2);
        void pop(int Amount = 2);

        void print(std::string_view Str) const;

        template<typename... Args>
        void print(fmt::format_string<Args...> Fmt, Args &&... Arguments) {
            printIndent();
            Out << fmt::format(Fmt, std::forward<Args>(Arguments)...);
        }

    private:
        std::ostream &Out;
        int IndentLevel = 0;

        void printIndent() const;
    };
} // namespace astra::support
