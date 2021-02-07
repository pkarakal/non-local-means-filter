#.rst:
# FindMatlab
# --------
# MATLAB® is a programming platform designed specifically for engineers
# and scientists. The heart of MATLAB is the MATLAB language, a matrix-based
# language allowing the most natural expression of computational mathematics.
#
# This will define the following variables::
#
# MATLAB_INCLUDE_DIRS - the MATLAB include directories
# MATLAB_LIBRARIES - the Matlab libraries
# MATLAB_FOUND - A boolean that checks if matlab is found


find_package(Matlab REQUIRED)
set(MATLAB_INCLUDE_DIRS ${Matlab_INCLUDE_DIRS})
set(MATLAB_LIBRARIES ${Matlab_LIBRARIES})
include_directories(SYSTEM ${Matlab_INCLUDE_DIRS} ${Matlab_ROOT_DIR}/extern/)
set(MATLAB_FOUND ${Matlab_FOUND})
mark_as_advanced(MATLAB_INCLUDE_DIRS MATLAB_LIBRARIES MATLAB_FOUND)
