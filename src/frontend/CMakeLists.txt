add_library(astra-frontend)

target_sources(astra-frontend
        PRIVATE
        ASTBuilder.cpp
        CompilerContext.cpp
        ConstantEvaluator.cpp
        SymbolCollector.cpp
        SymbolContext.cpp
        TypeContext.cpp
)

llvm_map_components_to_libnames(llvm_libs
        Support
)

target_include_directories(astra-frontend
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(astra-frontend
        PUBLIC
        astra-parser
        astra-support
        ${llvm_libs}
)