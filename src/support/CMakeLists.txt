add_library(astra-support
        Printer.cpp
)

add_library(astra::support ALIAS astra-support)

llvm_map_components_to_libnames(llvm_libs
        Support
)

target_include_directories(astra-support
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(astra-support
        PUBLIC
        fmt::fmt
        ${llvm_libs}
)
