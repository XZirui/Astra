antlr_target(AstraLexer
        AstraLexer.g4
        LEXER
        OUTPUT_DIRECTORY_PREFIX astra/parser
        PACKAGE astra::parser
)

antlr_target(AstraParser
        AstraParser.g4
        PARSER
        PACKAGE astra::parser
        DEPENDS_ANTLR AstraLexer
        OUTPUT_DIRECTORY_PREFIX astra/parser
        COMPILE_FLAGS -lib ${ANTLR_AstraLexer_OUTPUT_DIR}
        VISITOR
)

#message(${ANTLR_AstraLexer_OUTPUT_DIR})

include_directories(
        ${ANTLR_AstraLexer_OUTPUT_DIR}
        ${ANTLR_AstraParser_OUTPUT_DIR}
)

add_library(astra-parser STATIC
        ${ANTLR_AstraLexer_CXX_OUTPUTS}
        ${ANTLR_AstraParser_CXX_OUTPUTS}
)


target_include_directories(astra-parser PUBLIC
        ${ANTLR_AstraLexer_OUTPUT_DIR}
        ${ANTLR_AstraParser_OUTPUT_DIR}
)

target_link_libraries(astra-parser PUBLIC
        antlr4_static
)