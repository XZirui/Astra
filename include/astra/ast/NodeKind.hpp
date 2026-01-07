#pragma once

namespace astra::ast {

// Ignore the attributes
#include "macros/NodeMacrosEmpty.hpp"
#undef NODE
#define NODE(NAME, BASE, FIELDS) NAME,

    enum class NodeKind {
#include "AllNodes.def"

    };

#include "macros/NodeMacrosUndef.hpp"

} // namespace astra::ast