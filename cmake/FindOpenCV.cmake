#.rst:
# FindOpenCV
# --------
# Finds the OpenCV library
#
# This will define the following variables::
#
# OPENCV_INCLUDE_DIRS - the OpenCV include directories
# OPENCV_LIBRARIES - the OpenCV libraries
# OPENCV_FOUND - a boolean that indicates if OpenCV

if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_OPENCV opencv4 QUIET)
    find_library(PC_OPENCV_LIBRARIES
            NAMES opencv.hpp
            PATHS ${PC_OPENCV_INCLUDE_DIRS})
    set(OPENCV_LIBRABIES ${PC_OPENCV_LIBRARIES})
    find_path(OPENCV_INCLUDE_DIRS NAMES opencv.hpp
            PATHS ${PC_OPENCV_INCLUDE_DIRS})
    include_directories(${PC_OPENCV_INCLUDE_DIRS} ${PC_OPENCV_PREFIX}
            ${PC_OPENCV_LIBDIR})
    set(OPENCV_LIBRARIES ${PC_OPENCV_LIBRARIES})
    set(OPENCV_VERSION ${PC_OPENCV_VERSION})
    set(OPENCV_FOUND TRUE)
    message("-- Found OpenCV: ${PC_OPENCV_PREFIX} (found version ${PC_OPENCV_VERSION})")
else()
    find_package(OpenCV REQUIRED)
    set(OPENCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS})
    set(OPENCV_LIBRARIES ${OpenCV_LIBRARIES})
    set(OPENCV_VERSION ${OpenCV_VERSION})
    include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
    set(OPENCV_FOUND TRUE)
endif()

mark_as_advanced(OPENCV_INCLUDE_DIRS OPENCV_LIBRARIES OPENCV_FOUND)
