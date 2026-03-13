cmake_minimum_required(VERSION 3.16)

project(tlb_size_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

find_package(Catch2 3 QUIET)

add_library(tlb_bench_lib
    src/benchmark.cpp
    src/timer.cpp
)

target_include_directories(tlb_bench_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(tlb_bench
    src/main.cpp
)

target_link_libraries(tlb_bench PRIVATE
    tlb_bench_lib
)

if (Catch2_FOUND)
    add_executable(tlb_bench_tests
        tests/catch_entry.cpp
        tests/unit.cpp
    )

    target_link_libraries(tlb_bench_tests PRIVATE
        tlb_bench_lib
        Catch2::Catch2
    )

    add_test(NAME tlb_bench_tests COMMAND tlb_bench_tests)
endif()