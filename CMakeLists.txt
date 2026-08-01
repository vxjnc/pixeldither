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

add_subdirectory(godot-cpp)

file(GLOB SOURCES src/*.cpp)

add_library(pixeldither SHARED ${SOURCES})
target_include_directories(pixeldither PRIVATE src)

target_link_libraries(pixeldither PRIVATE godot::cpp)

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

add_custom_command(TARGET pixeldither POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/project/bin/${PLATFORM_LOWER}"
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:pixeldither>"
        "${CMAKE_SOURCE_DIR}/project/bin/${PLATFORM_LOWER}/"
)

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json" AND NOT "${CMAKE_BINARY_DIR}/compile_commands.json" STREQUAL "${CMAKE_SOURCE_DIR}/build/compile_commands.json")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/build/compile_commands.json"
        COPY_ON_ERROR SYMBOLIC
    )
endif()
