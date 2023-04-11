from conans import ConanFile, CMake

class GslLiteConan(ConanFile):
    version = "0.41.0"
    name = "gsl-lite"
    description = "A single-file header-only version of ISO C++ Guidelines Support Library (GSL) for C++98, C++11 and later"
    license = "MIT License. https://opensource.org/licenses/MIT"
    url = "https://github.com/gsl-lite/gsl-lite.git"
    exports_sources = "include/gsl/gsl-lite.hpp", "include/gsl-lite/gsl-lite.hpp", "CMakeLists.txt", "cmake/*", "LICENSE"
    settings = "compiler", "build_type", "arch"
    build_policy = "missing"
    author = "Martin Moene"

    def build(self):
        """Avoid warning on build step"""
        pass

    def package(self):
        """Run CMake install"""
        cmake = CMake(self)
        cmake.configure()
        cmake.install()

    def package_info(self):
        self.info.header_only()
