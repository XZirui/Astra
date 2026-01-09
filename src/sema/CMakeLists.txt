add_library(astra-sema)

target_sources(astra-sema
        PRIVATE
        SymbolTable.cpp
)

llvm_map_components_to_libnames(llvm_libs
        Support
)

target_include_directories(astra-sema
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(astra-sema
        PUBLIC
        astra-frontend
        astra-support
        ${llvm_libs}
)