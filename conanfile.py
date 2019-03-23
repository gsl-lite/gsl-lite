from conans import ConanFile

class GslLiteConan(ConanFile):
    version = "0.34.0"
    name = "gsl-lite"
    description = "A single-file header-only version of ISO C++ Guidelines Support Library (GSL) for C++98, C++11 and later"
    license = "MIT License. https://opensource.org/licenses/MIT"
    url = "https://github.com/martinmoene/gsl-lite.git"
    exports_sources = "include/gsl/*", "include/*", "LICENSE"
    build_policy = "missing"    
    author = "Martin Moene"

    def build(self):
        """Avoid warning on build step"""
        pass

    def package(self):
        """Provide pkg/include/gsl/*.hpp"""
        self.copy("*.hpp")
        self.copy("include/gsl/gsl")

    def package_info(self):
        self.info.header_only()
