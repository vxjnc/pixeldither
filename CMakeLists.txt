cmake_minimum_required(VERSION 3.19)
project(pixeldither)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# --- 1. Pure C++ core library ---
add_library(pixeldither_core INTERFACE)
target_include_directories(pixeldither_core INTERFACE src)

# --- 2. Godot GDExtension library ---
add_subdirectory(godot-cpp)

file(GLOB GODOT_SOURCES src/godot/*.cpp)

add_library(pixeldither SHARED ${GODOT_SOURCES})
target_include_directories(pixeldither PRIVATE src/godot)

# --- Link core library and godot-cpp bindings ---
target_link_libraries(pixeldither PRIVATE pixeldither_core godot::cpp)

# --- Configure target naming and output paths for Godot ---
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
if(BUILD_TYPE_LOWER STREQUAL "release")
    set(GODOT_TARGET_SUFFIX "template_release")
else()
    set(GODOT_TARGET_SUFFIX "template_debug")
endif()

string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM_LOWER)

set_target_properties(pixeldither PROPERTIES
    OUTPUT_NAME "pixeldither.${PLATFORM_LOWER}.${GODOT_TARGET_SUFFIX}.x86_64"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_LOWER}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_LOWER}"
)

# --- Copy compiled binary to Godot project folder ---
add_custom_command(TARGET pixeldither POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/project/bin/${PLATFORM_LOWER}"
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:pixeldither>"
        "${CMAKE_SOURCE_DIR}/project/bin/${PLATFORM_LOWER}/"
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

    add_executable(pixeldither_bench ${BENCH_SOURCES})
    target_link_libraries(pixeldither_bench PRIVATE pixeldither_core benchmark::benchmark)

    set_target_properties(pixeldither_bench PROPERTIES
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
