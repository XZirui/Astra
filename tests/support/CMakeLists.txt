add_executable(astra-tests-support
        SourceRange.cpp
)

target_link_libraries(astra-tests-support
        PRIVATE
        astra-support
        astra-tests-mocks
        Catch2::Catch2WithMain
)

catch_discover_tests(astra-tests-support)