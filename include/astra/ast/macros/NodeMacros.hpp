// ReSharper disable once CppMissingIncludeGuard
// No pragma once or include guard here!

#define ATTR(TYPE, NAME) TYPE NAME;
#define ATTR_LIST(TYPE, NAME) std::vector<TYPE> NAME{};

#define ABSTRACT_NODE(NAME, BASE, ...)                                         \
    struct NAME : BASE {                                                       \
        __VA_ARGS__                                                            \
                                                                               \
        using BASE::BASE;                                                      \
    };

#define NODE(NAME, BASE, FIELDS)                                               \
    struct NAME final : BASE {                                                 \
        FIELDS                                                                 \
                                                                               \
        using BASE::BASE;                                                      \
        NodeKind getKind() const override { return NodeKind::NAME; }           \
    };
