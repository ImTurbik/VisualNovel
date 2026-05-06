cmake_minimum_required(VERSION 3.20)
project(VisualNovel)

set(CMAKE_CXX_STANDARD 17)

set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL/cmake")

find_package(SDL3 REQUIRED)

add_executable(VisualNovel
    src/main.cpp
    src/Game.cpp
    src/Game.hpp
)

target_include_directories(VisualNovel PRIVATE src)

target_link_libraries(VisualNovel PRIVATE SDL3::SDL3)

add_custom_command(TARGET VisualNovel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/SDL/lib/x64/SDL3.dll"
    $<TARGET_FILE_DIR:VisualNovel>)
