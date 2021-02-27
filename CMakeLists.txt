cmake_minimum_required(VERSION 3.17)
project(non_local_means_filter LANGUAGES CXX CUDA)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)


option(ENABLE_CLANGTIDY "Enable clang-tidy support?" OFF)
option(ENABLE_PYBIND11 "Enable python bindings support?" ON)
option(ENABLE_OCTAVE "Enable Octave support?" OFF)
option(ENABLE_MATLAB "Enable Matlab support?" OFF)
option(ENABLE_OPENCV "Enable OpenCV support" ON)
option(ENABLE_BOOST "Enable Boost support" ON)

if (ENABLE_CLANGTIDY)
    include(cmake/FindClangTidy.cmake)
endif ()

if(ENABLE_OCTAVE)
    include(cmake/FindOctave.cmake)
    if(OCTAVE_FOUND)
        link_libraries(${OCTAVE_LIBRARIES})
        set(USE_OCTAVE 1)
    endif()
endif()

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

if(ENABLE_OPENCV)
    include(cmake/FindOpenCV.cmake)
    if(OPENCV_FOUND)
        link_libraries(${OPENCV_LIBRARIES})
        find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
        target_link_libraries(nlm_lib Qt5::Widgets Qt5::Core)
        set(USE_OPENCV 1)
    endif()
endif()

if(NOT DEFINED ${CMAKE_CUDA_ARCHITECTURES})
    set(CMAKE_CUDA_ARCHITECTURES 52 61 75)
endif()

if(ENABLE_BOOST)
    include(cmake/FindBoost.cmake)
    if(BOOST_FOUND)
        link_libraries(${BOOST_LIBRARIES})
        set(USE_BOOST 1)
    endif()
endif()

configure_file(lib/definitions.h.in lib/definitions.h)
add_executable(non_local_means_filter main.cu)
target_link_libraries(non_local_means_filter nlm_lib)

if(ENABLE_OCTAVE AND OCTAVE_FOUND)
    target_link_libraries(non_local_means_filter ${OCTAVE_LIBS})
endif()

set_target_properties(
        non_local_means_filter
        PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
