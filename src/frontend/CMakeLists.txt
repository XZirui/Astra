add_library(astra-frontend)

target_sources(astra-frontend
        PRIVATE
        ASTBuilder.cpp
        CompilerContext.cpp
        TypeContext.cpp
)

target_include_directories(astra-frontend
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(astra-frontend
        PUBLIC
        astra-parser
        astra-support
)