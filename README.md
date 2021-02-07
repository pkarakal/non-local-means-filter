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
7. pkgconf (Optional)
8. Octave (Optional)
9. Matlab (Optional)
10. OpenCV (Optional)

To install them on variant Linux distributions follow the instructions below

#### Fedora
```shell
# add negativo17 repository for nvidia drivers and cuda
$ sudo dnf config-manager --add-repo=https://negativo17.org/repos/fedora-nvidia.repo
$ sudo dnf upgrade --refresh # updates installed packages and repositories metadata
$ sudo dnf install cmake make gcc-c++ pkgconf python3 nvidia-driver nvidia-driver-cuda \
  cuda-devel python3-pybind11  octave octave-devel opencv opencv-devel
```

#### Ubuntu
```shell
# updates installed packages and repositories metadata
$ sudo apt-get update && sudo apt-get upgrade
# installs NVIDIA drivers
$ sudo apt install ubuntu-drivers-common && ubuntu-drivers install nvidia-driver-460
# installs dependencies
$ sudo apt install cmake make g++  pkg-config nvidia-cuda-toolkit \ 
  python-pybind11 octave liboctave-dev libopencv-dev
```

To install Matlab, visit https://www.mathworks.com/
