add_library(astra-tests-mocks INTERFACE)

target_include_directories(astra-tests-mocks
        INTERFACE
        ${PROJECT_SOURCE_DIR}/tests
)
