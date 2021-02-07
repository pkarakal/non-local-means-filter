cmake_minimum_required(VERSION 3.17)
project(non_local_means_filter LANGUAGES CXX CUDA)
set(CMAKE_CUDA_STANDARD 11)

find_package(PkgConfig)


option(ENABLE_CLANGTIDY "Enable clang-tidy support?" OFF)
option(ENABLE_PYBIND11 "Enable python bindings support?" ON)

if (ENABLE_CLANGTIDY)
    include(FindClangTidy.cmake)
endif ()


if(ENABLE_PYBIND11)
    find_package(pybind11 REQUIRED)
    add_executable(converter converter.cpp)
    target_link_libraries(converter PUBLIC pybind11::module pybind11::lto pybind11::embed)
    set(USE_PARSED_FILES 1)
endif()

add_library(rapidcsv OBJECT lib/rapidcsv.h)

add_executable(non_local_means_filter main.cu)

set_target_properties(
        non_local_means_filter
        PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
