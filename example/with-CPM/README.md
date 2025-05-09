# gsl-lite example project (CPM)


This is a simple CMake project that demonstrates how to use *gsl-lite* with the [CPM](https://github.com/cpm-cmake/CPM.cmake) package manager.

Requirements:

- [CMake](https://cmake.org/) 3.20 or newer must be in the path.
- A C++ compiler must be installed and available in the path.
- An internet connection must be available to allow CPM to retrieve the *gsl-lite* dependency from GitHub.

To set up the project, open a command-line window, navigate to the directory containing this readme, and execute the following commands:


## Configure
```
cmake --config --preset default
```


## Build
```
cmake --build build/default
./build/default/program
```

## Run
```
./build/default/program
```
