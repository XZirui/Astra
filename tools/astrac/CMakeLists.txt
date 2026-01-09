add_executable(astrac main.cpp)

target_link_libraries(astrac
        PRIVATE
        antlr4_static
        astra-frontend
        astra-parser
        astra-sema
        astra-support
        astra-tools-ast
)