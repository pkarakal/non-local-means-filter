cmake_minimum_required(VERSION 3.17)
project(non_local_means_filter LANGUAGES CXX CUDA)
set(CMAKE_CUDA_STANDARD 11)

find_package(PkgConfig)


option(ENABLE_CLANGTIDY "Enable clang-tidy support?" OFF)
option(ENABLE_PYBIND11 "Enable python bindings support?" ON)
option(ENABLE_MATLAB "Enable Matlab support?" OFF)

if (ENABLE_CLANGTIDY)
    include(cmake/FindClangTidy.cmake)
endif ()

if(ENABLE_MATLAB)
    include(cmake/FindMatlab.cmake)
    if(MATLAB_FOUND)
        link_libraries(${MATLAB_LIBRARIES} mat)
        set(USE_MATLAB 1)
    endif()
endif()

if(ENABLE_PYBIND11)
    find_package(pybind11 REQUIRED)
    add_executable(converter converter.cpp)
    add_executable(preprocessor preprocessor.cpp)
    target_link_libraries(converter PUBLIC pybind11::module pybind11::lto pybind11::embed)
    target_link_libraries(preprocessor PUBLIC pybind11::module pybind11::lto pybind11::embed)
    set(USE_PARSED_FILES 1)
endif()


add_library(nlm_lib OBJECT lib/nlm_lib.cpp lib/nlm_lib.h)
add_library(rapidcsv OBJECT lib/rapidcsv.h)
target_link_libraries(rapidcsv)

add_executable(non_local_means_filter main.cu)
if (ENABLE_IMAGEMAGICK AND IMAGEMAGICK_FOUND)
    target_link_libraries(main ${IMAGEMAGICK_LIBRARIES})
    target_link_libraries(non_local_means_filter ${IMAGEMAGICK_LIBRARIES})
endif()

if(ENABLE_OCTAVE AND OCTAVE_FOUND)
    target_link_libraries(main ${OCTAVE_LIBS})
endif()

set_target_properties(
        non_local_means_filter
        PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
