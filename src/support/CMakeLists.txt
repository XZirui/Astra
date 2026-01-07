add_library(astra-support
        Printer.cpp
)

add_library(astra::support ALIAS astra-support)

target_include_directories(astra-support
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(astra-support
        PUBLIC
        fmt::fmt
)
