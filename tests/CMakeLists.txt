add_executable(astra_tests
    test_ast.cpp
)

target_link_libraries(astra_tests
    PRIVATE
    astra-ast
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(astra_tests)