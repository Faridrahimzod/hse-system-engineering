cmake_minimum_required(VERSION 3.16)

project(mutex_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Catch2 3 REQUIRED)

add_executable(mutex_tests
    tests/catch_entry.cpp
    tests/unit.cpp
    tests/stress.cpp
)

target_include_directories(mutex_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mutex_tests PRIVATE
    Catch2::Catch2
    Threads::Threads
)