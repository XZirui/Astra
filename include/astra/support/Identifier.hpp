#pragma once

#include <llvm/ADT/StringRef.h>

namespace astra::support {
    struct Identifier {
        Identifier() = default;
        explicit Identifier(llvm::StringRef S) : Name(S) {}

        [[nodiscard]] llvm::StringRef getName() const { return Name; }

    private:
        llvm::StringRef Name;
    };
} // namespace astra::support