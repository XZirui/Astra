#pragma once

#include "Node.hpp"

namespace astra::ast {
    enum class Op { Add, Sub, Mult, Div, Mod, Eq, Neq, Lt, Gt, Le, Ge, Disj, Conj }; // TODO
#include "macros/NodeMacros.hpp"
#include "ExprNodes.def"
#include "macros/NodeMacrosUndef.hpp"
}