add_library(astra-tests-mocks INTERFACE)

target_include_directories(astra-tests-mocks
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
