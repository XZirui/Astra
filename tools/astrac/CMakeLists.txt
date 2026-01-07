add_executable(astrac main.cpp)

target_link_libraries(astrac
        PRIVATE
        antlr4_static
        astra-frontend
        astra-parser
        astra-support
        astra-tools-ast
)