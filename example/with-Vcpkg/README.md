# gsl-lite example project (Vcpkg)


This is a simple CMake project that demonstrates how to use *gsl-lite* with the [Vcpkg](https://vcpkg.io/en/package/gsl-lite) package manager.

Requirements:

- [CMake](https://cmake.org/) 3.20 or newer must be in the path.
- A C++ compiler must be installed and available in the path.
- Vcpkg must be installed, and the `VCPKG_ROOT` environment variable must refer to the Vcpkg root directory.
- An internet connection must be available to allow Vcpkg to retrieve the *gsl-lite* dependency from GitHub.

To set up the project, open a command-line window, navigate to the directory containing this readme, and execute the following commands:


## Configure
```
cmake --preset default
```


## Build
```
cmake --build build/default --config Debug
```

## Run

| Windows                          | Linux, MacOS                       |
|----------------------------------|------------------------------------|
| `build\default\Debug\my-program` | `./build/default/Debug/my-program` |
