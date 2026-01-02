#pragma once

#include "Stmt.hpp"
#include "Type.hpp"

namespace astra::ast {
    // TODO
    enum class ParamType { Default };

#include "macros/NodeMacros.hpp" // NOLINT(*-include-order)
#include "DeclNodes.def"
#include "macros/NodeMacrosUndef.hpp"
} // namespace astra::ast
