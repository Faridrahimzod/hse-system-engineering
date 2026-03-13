cmake_minimum_required(VERSION 3.16)

project(mycpu_fuse_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

find_package(Catch2 3 QUIET)
find_package(Doxygen QUIET)

add_executable(mycpu_dummy_tests
    tests/catch_entry.cpp
    tests/unit.cpp
)

target_include_directories(mycpu_dummy_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (Catch2_FOUND)
    target_link_libraries(mycpu_dummy_tests PRIVATE Catch2::Catch2)
endif()

add_test(NAME mycpu_dummy_tests COMMAND mycpu_dummy_tests)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/cpuemu
    ${CMAKE_CURRENT_BINARY_DIR}/cpuemu
    COPYONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/cpuemufs.py
    ${CMAKE_CURRENT_BINARY_DIR}/cpuemufs.py
    COPYONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/device.py
    ${CMAKE_CURRENT_BINARY_DIR}/device.py
    COPYONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/test.py
    ${CMAKE_CURRENT_BINARY_DIR}/test.py
    COPYONLY
)

if (DOXYGEN_FOUND)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )
endif()

install(PROGRAMS
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/cpuemu
    DESTINATION bin
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/cpuemufs.py
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/device.py
    DESTINATION share/mycpu-fuse-project
)

set(CPACK_PACKAGE_NAME "mycpu-fuse-project")
set(CPACK_PACKAGE_VENDOR "Farid")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FUSE-based emulator of a simple CPU device tree")
set(CPACK_GENERATOR "TGZ")
include(CPack)