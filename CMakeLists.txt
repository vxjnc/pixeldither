cmake_minimum_required(VERSION 3.19)

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CLANGXX_EXECUTABLE NAMES clang++)
    if(CLANGXX_EXECUTABLE)
        set(CMAKE_CXX_COMPILER "${CLANGXX_EXECUTABLE}")
        message(STATUS "Clang not explicitly set, defaulting to found clang++: ${CLANGXX_EXECUTABLE}")
    else()
        message(WARNING "clang++ not found in PATH, falling back to default compiler")
    endif()
endif()

if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CLANG_EXECUTABLE NAMES clang)
    if(CLANG_EXECUTABLE)
        set(CMAKE_C_COMPILER "${CLANG_EXECUTABLE}")
        message(STATUS "Clang not explicitly set, defaulting to found clang: ${CLANG_EXECUTABLE}")
    else()
        message(WARNING "clang not found in PATH, falling back to default compiler")
    endif()
endif()

project(pixeldither)
set(ADDON_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

if(MSVC)
    add_compile_options(/vmg)
endif()

# --- 1. Pure C++ core library ---
add_library(${ADDON_NAME}_core INTERFACE)
target_include_directories(${ADDON_NAME}_core INTERFACE src)

# --- 2. Godot GDExtension library ---
add_subdirectory(godot-cpp)

file(GLOB GODOT_SOURCES src/godot/*.cpp)

add_library(${ADDON_NAME} SHARED ${GODOT_SOURCES})
target_include_directories(${ADDON_NAME} PRIVATE src/godot)

# --- Link core library and godot-cpp bindings ---
target_link_libraries(${ADDON_NAME} PRIVATE ${ADDON_NAME}_core godot::cpp)

# --- Configure target naming and output paths for Godot ---
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
if(BUILD_TYPE_LOWER STREQUAL "release")
    set(GODOT_TARGET_SUFFIX "template_release")
else()
    set(GODOT_TARGET_SUFFIX "template_debug")
endif()

string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM_LOWER)

set_target_properties(${ADDON_NAME} PROPERTIES
    OUTPUT_NAME "${ADDON_NAME}.${PLATFORM_LOWER}.${GODOT_TARGET_SUFFIX}.x86_64"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/project/addons/${ADDON_NAME}/bin/${PLATFORM_LOWER}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/project/addons/${ADDON_NAME}/bin/${PLATFORM_LOWER}"
)

# --- 3. Google Benchmark (optional target) ---
option(BUILD_BENCHMARKS "Build Google Benchmark executable" OFF)

if(BUILD_BENCHMARKS)
    include(FetchContent)

    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.9.5
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)

    file(GLOB BENCH_SOURCES src/bench/*.cpp)

    add_executable(${ADDON_NAME}_bench ${BENCH_SOURCES})
    target_link_libraries(${ADDON_NAME}_bench PRIVATE ${ADDON_NAME}_core benchmark::benchmark)

    set_target_properties(${ADDON_NAME}_bench PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# --- Create symlink for compile_commands.json ---
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json" AND NOT "${CMAKE_BINARY_DIR}/compile_commands.json" STREQUAL "${CMAKE_SOURCE_DIR}/build/compile_commands.json")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/build/compile_commands.json"
        COPY_ON_ERROR SYMBOLIC
    )
endif()
