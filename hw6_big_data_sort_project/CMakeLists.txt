cmake_minimum_required(VERSION 3.16)

project(big_data_sort_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

find_package(Threads REQUIRED)
find_package(Catch2 3 QUIET)

add_library(big_data_sort_lib
    src/file_mapping.cpp
    src/external_sort.cpp
)

target_include_directories(big_data_sort_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(big_data_sort_lib PUBLIC
    Threads::Threads
)

add_executable(sort_data
    src/main.cpp
)

target_link_libraries(sort_data PRIVATE
    big_data_sort_lib
)

add_executable(generate_data
    tools/generate_data.cpp
)

target_include_directories(generate_data PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(verify_sorted
    tools/verify_sorted.cpp
)

target_link_libraries(verify_sorted PRIVATE
    big_data_sort_lib
)

target_include_directories(verify_sorted PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (Catch2_FOUND)
    add_executable(big_data_sort_tests
        tests/catch_entry.cpp
        tests/unit.cpp
    )

    target_link_libraries(big_data_sort_tests PRIVATE
        big_data_sort_lib
        Catch2::Catch2
    )

    add_test(NAME big_data_sort_tests COMMAND big_data_sort_tests)
endif()