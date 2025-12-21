cmake_minimum_required(VERSION 3.20)
project(
        astra
        VERSION 0.1.0
        LANGUAGES CXX
)

# Global C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_TOOLS "Build tools" ON)
option(BUILD_TESTS "Build tests" ON)

# Warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif ()

# Include paths
include_directories(${PROJECT_SOURCE_DIR}/include)

# Third-party dependencies
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Antlr4
include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})
find_package(ANTLR REQUIRED)

# Catch2
if (BUILD_TESTS)
    Include(FetchContent)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.8.1 # or a later release
    )
    FetchContent_MakeAvailable(Catch2)
endif ()

add_subdirectory(src)
if (BUILD_TOOLS)
    add_subdirectory(tools)
endif ()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()
