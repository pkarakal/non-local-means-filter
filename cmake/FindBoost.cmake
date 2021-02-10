#.rst:
# FindBoost
# --------
# Boost is a set of libraries for the C++
# programming language that provides support
# for tasks and structures such as linear algebra,
# pseudorandom number generation, multithreading,
# image processing, regular expressions, and unit testing.
#
# This will define the following variables::
#
# BOOST_INCLUDE_DIRS - the Boost include directories
# BOOST_LIBRARIES - the Boost libraries
# BOOST_FOUND - A boolean that checks if Boost is found


find_package(Boost 1.70.0 REQUIRED COMPONENTS program_options)
set(BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(BOOST_LIBRARIES ${Boost_LIBRARIES})
include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${Boost_LIBRARY_DIRS})
set(BOOST_FOUND ${Boost_FOUND})
mark_as_advanced(BOOST_INCLUDE_DIRS BOOST_LIBRARIES BOOST_FOUND)
