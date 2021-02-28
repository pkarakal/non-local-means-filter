#.rst:
# FindOpenMP
# --------
# Finds OpenMP API
#
# Here we are searching for the OpenMP API .
# If found it adds the necessary compile flags
# This will define the following variables:
#
# OPENMP_FOUND - a boolean that indicates if OpenMP is found

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    set(OPENMP_FOUND ${OpenMP_CXX_FOUND})
    add_compile_options(-Xcompiler -fopenmp)
endif ()

mark_as_advanced(OPENMP_FOUND)