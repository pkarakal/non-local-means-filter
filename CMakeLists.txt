cmake_minimum_required(VERSION 3.17)
project(non_local_means_filter LANGUAGES CXX CUDA)
set(CMAKE_CUDA_STANDARD 11)

option(ENABLE_CLANGTIDY "Enable clang-tidy support?" OFF)

if (ENABLE_CLANGTIDY)
    include(FindClangTidy.cmake)
endif ()

add_executable(non_local_means_filter main.cu)

set_target_properties(
        non_local_means_filter
        PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
