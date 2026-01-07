#pragma once

#include <string_view>

namespace astra::support {
    struct Identifier {
        explicit Identifier(std::string_view S) : Name(S) {}

        [[nodiscard]] std::string_view getName() const { return Name; }

    private:
        std::string_view Name;
    };
} // namespace astra::support