cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()

project(PlyLoaderAll)

# Set to C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# find the gtest package
find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
# this will find all the gtest functions
include(GoogleTest)

enable_testing()
add_executable(TestPlyLoader)
target_sources(TestPlyLoader PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/TestPlyLoader.cpp
        ${CMAKE_SOURCE_DIR}/src/PlyLoader.cpp
        ${CMAKE_SOURCE_DIR}/include/PlyLoader.h
)
target_link_libraries(TestPlyLoader PRIVATE GTest::gtest_main  fmt::fmt-header-only)
target_link_libraries(TestPlyLoader PRIVATE range-v3::meta range-v3::concepts range-v3::range-v3)

target_include_directories(TestPlyLoader PRIVATE ./include)
gtest_discover_tests(TestPlyLoader)


add_custom_target(TestPlyLoaderCopyTestFiles ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/files
        ${PROJECT_BINARY_DIR}/files
)

ADD_DEPENDENCIES(TestPlyLoader TestPlyLoaderCopyTestFiles)
