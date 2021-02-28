# non-local-mean filter
CUDA implementation of non-local-means filter. This project is implemented
as a part of a homework exercise for [050] - Parallel & Distributed Systems course
of ECE Department, AUTh.

More information, as project progresses

## Getting Started

### Prerequisites
1. CMake
2. Make
3. g++
4. CUDA
5. Python
6. PyBind11
7. Boost 
8. pkgconf (Optional)
9. Octave (Optional)
10. Matlab (Optional)
11. OpenCV (Optional)
12. OpenMP (Optional)

To install them on variant Linux distributions follow the instructions below

#### Fedora
```shell
# add negativo17 repository for nvidia drivers and cuda
$ sudo dnf config-manager --add-repo=https://negativo17.org/repos/fedora-nvidia.repo
$ sudo dnf upgrade --refresh # updates installed packages and repositories metadata
$ sudo dnf install cmake make gcc-c++ pkgconf python3 nvidia-driver nvidia-driver-cuda \
  cuda-devel python3-pybind11  octave octave-devel opencv opencv-devel boost boost-devel \
  libomp-devel
```

#### Ubuntu
```shell
# updates installed packages and repositories metadata
$ sudo apt-get update && sudo apt-get upgrade
# installs NVIDIA drivers
$ sudo apt install ubuntu-drivers-common && ubuntu-drivers install nvidia-driver-460
# installs dependencies
$ sudo apt install cmake make g++  pkg-config nvidia-cuda-toolkit \ 
  python-pybind11 octave liboctave-dev libopencv-dev libboost-all-dev \
  libomp-dev
```

To install Matlab, visit https://www.mathworks.com/


### Instructions
1.  Clone the repository
    ```shell script
    $ git clone git@github.com:pkarakal/non-local-means-filter.git
    ```
2.  Go to that directory
    ```shell script
    $ cd non-local-means-filter/
    ```
3.  a. Generate Makefiles from the CMakefiles.txt
    ```shell script
    $ cmake -S .
    ```
    b. There are additional cmake files that import OpenMP, OpenCV, Boost, Matlab
    and Octave and there is also the option  to include ClangTidy support.
    To enable the build of the executable that leverages OpenMP run
    ```shell script
    $ cmake -S . -DENABLE_OPENMP=ON
    ``` 
    To enable the build of the executable that leverages OpenCV run
    ```
    $ cmake -S . -DENABLE_OPENCV=ON
    ```
    To enable the build of the executable that leverages Octave run
    ```
    $ cmake -S . -DENABLE_OCTAVE=ON
    ```
    To enable the build of the executable that leverages Matlab run
    ```
    $ cmake -S . -DENABLE_MATLAB=ON
    ```
    **Note**: __*DO NOT*__ enable both Matlab and OpenCV at the same time.
    Their Qt5 libraries are known to not being compatible
    
    To enable Clang Tidy support use the following flag
    ```shell script
    $ cmake -S . -DENABLE_CLANGTIDY=ON
    ```
4.  Build and run the application
    ```shell script
    $ cmake --build . && ./non_local_means_filter [...args]
    ```
6.  There is also a converter program that converts files in mat or image
    format to csv excluding the "Label" column. This is calls python
    under the hood for reading and converting to csv. To compile it run
    ```shell
    $ pip3 install -r requirements.txt
    $ cmake -S . -DENABLE_PYBIND11=ON
    $ make converter
    ```
    To run it use the following format:
    ```shell
    $ ./converter /path/to/file 
    # or for multiple files
    $ ./converter [ /path/to/file1 /path/to/file2 /path/to/fileN ]
    ```
    Make sure you have a python >=3.6 interpreter installed and in your PATH.
7.  There is, finally, a preprocessor for CSV files that removes headers from them,
    and selects only numeric data which it converts to `float64`. When item is NAN,
    it generates a random number for it using a python lambda. To compile it run
    ```shell
    $ pip3 install -r requirements.txt
    $ cmake -S . -DENABLE_PYBIND11=ON
    $ make preprocessor
    ```
    To run it use the following format:
    ```shell
    $ ./preprocessor /path/to/file <rows_to_skip> "<delimeter>" 
    # or for multiple files
    $ ./converter  /path/to/file1 <rows_to_skip> "<delimeter>" \
       /path/to/file2 <rows_to_skip> "<delimeter>" \
       /path/to/fileN <rows_to_skip> "<delimeter>" 
    ```