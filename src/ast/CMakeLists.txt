add_library(astra-ast)

target_sources(astra-ast
        PRIVATE
        ASTBuilder.cpp
        ASTContext.cpp
)

target_include_directories(astra-ast
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(astra-ast
        PUBLIC
        astra-parser
)
