add_library(astra-tools-ast)

target_sources(astra-tools-ast
        PRIVATE
        ASTDumper.cpp
)

target_include_directories(astra-tools-ast
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(astra-tools-ast
        PUBLIC
        astra-support
)