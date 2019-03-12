File: gsl-lite/example/cmake-pkg/Readme.md

One example of installing gsl-lite package (on Windows):
```
prompt> cd ...\gsl-lite
prompt> script\install-gsl-pkg.py --install-prefix d:/dev2
Installing gsl-lite package:

cmake -H. -B"./cmake-pkg-install" -G"Unix Makefiles"  -DCMAKE_CXX_COMPILER="g++" -DCMAKE_INSTALL_PREFIX="d:/dev2"
-- Configuring done
-- Generating done
-- Build files have been written to: D:/Own/Martin/Cloud/Dropbox/Project/GitHub-Pre/IsoCpp/gsl-lite/cmake-pkg-install
cmake --build ./cmake-pkg-install   --target install
Install the project...
-- Install configuration: ""
-- Installing: D:/dev2/lib/cmake/gsl-lite/gsl-liteTargets.cmake
-- Installing: D:/dev2/include
-- Installing: D:/dev2/include/gsl
-- Installing: D:/dev2/include/gsl/gsl
-- Installing: D:/dev2/include/gsl/gsl-lite.h
-- Installing: D:/dev2/include/gsl/gsl-lite.hpp
-- Installing: D:/dev2/include/gsl.h
-- Installing: D:/dev2/include/gsl.hpp
-- Installing: D:/dev2/lib/cmake/gsl-lite/gsl-liteConfig.cmake
-- Installing: D:/dev2/lib/cmake/gsl-lite/gsl-liteConfigVersion.cmake
```

Use package:
```
prompt> cd example/cmake-pkg
prompt> use-gsl-pkg.py --cmake-prefix d:/dev2
Using gsl-lite package:

cmake -H. -B"./cmake-pkg-build" -G"Unix Makefiles"  -DCMAKE_CXX_COMPILER="g++" -DCMAKE_INSTALL_PREFIX="_stage" -DCMAKE_PREFIX_PATH="d:/dev2"
-- The CXX compiler identification is GNU 7.2.0
-- Check for working CXX compiler: C:/MinGW/bin/g++.exe
-- Check for working CXX compiler: C:/MinGW/bin/g++.exe -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: .../gsl-lite/example/cmake-pkg/cmake-pkg-build
cmake --build ./cmake-pkg-build
Scanning dependencies of target package_using_gsl-lite
[ 50%] Building CXX object CMakeFiles/package_using_gsl-lite.dir/main.cpp.obj
[100%] Linking CXX executable package_using_gsl-lite.exe
[100%] Built target package_using_gsl-lite
ctest
Test project .../gsl-lite/example/cmake-pkg/cmake-pkg-build
    Start 1: test
1/1 Test #1: test .............................   Passed    0.04 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.06 sec
```