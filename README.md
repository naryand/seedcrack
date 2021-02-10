# seedcrack
Allows for finding the lower 48 bits of a Minecraft world seed quickly using
* Slime chunks
* Emerald ores
* Other upcoming features such as diorite

with multithreaded CPU code as well as capability for using diamond ore locations on the GPU in CUDA/OpenCL.

A user interface is currently not implemented. Feature data is entered into an array of feature structs named `arr`. Once data is entered into `arr`, compile and run the program as indicated below to start searching for seeeds.

## CPU
CPU code is written in C using POSIX threads and compiles on Linux using gcc with `make` in the directory. 

On Windows, use [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install-win10) as a Linux environment. 

macOS should be able to use clang to compile and run the code, but this is untested.

## GPU
GPU code is written in both CUDA and OpenCL.

### NVIDIA
On NVIDIA graphics, CUDA is preferred for a slight perfomance increase.

On Linux, apt install `nvcc` and compile in the `cuda/` directory using nvcc with `make` and run `./diamond`

On Windows, install the [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads) and add the Visual Studio MSVC compiler to your path, then compile on the command line with `nvcc`.

On macOS, CUDA is not supported. Refer to the AMD section and use OpenCL.

### AMD
On Linux, ensure the opencl-headers are installed, and compile in the `opencl/` directory using gcc with `make` and run `./main`. 

On Windows, use Visual Studio with the [OpenCL.dll and OpenCL headers](https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/releases) set up to compile and run.

macOS should be able to compile with `clang -framework OpenCL`, but this is untested.
