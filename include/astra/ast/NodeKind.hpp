#pragma once

namespace astra::ast {

// Ignore the attributes
#define ATTR(TYPE, NAME)
#define ATTR_LIST(TYPE, NAME)
#define ABSTRACT_NODE(NAME, BASE, ...)

#define NODE(NAME, BASE, FIELDS) NAME,

    enum class NodeKind {
#include "AllNodes.def"
    };

#include "macros/NodeMacrosUndef.hpp"

} // namespace astra::ast