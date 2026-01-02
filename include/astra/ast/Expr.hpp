#pragma once

#include "Node.hpp"

namespace astra::ast {
    enum class Op { Add, Sub, Mult, Div, Mod, Eq, Neq, Lt, Gt, Le, Ge, Disj, Conj }; // TODO
    enum class FloatType { Float, Double };

#include "macros/NodeMacros.hpp" // NOLINT(*-include-order)
#include "ExprNodes.def"
#include "macros/NodeMacrosUndef.hpp"
} // namespace astra::ast
