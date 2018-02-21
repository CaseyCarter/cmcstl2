from conans import ConanFile, CMake
from conans.tools import download, unzip
from conans.client.runner import ConanRunner
from io import StringIO

def git_commit_hash():
    hash_buf = StringIO()
    ConanRunner()("git rev-parse --short HEAD", output=hash_buf)
    return hash_buf.getvalue()

class CMCSTL2Conan(ConanFile):
    name = "cmcstl2"
    version = git_commit_hash()
    url="https://github.com/CaseyCarter/cmcstl2"
    description="An implementation of C++ Extensions for Ranges"
    license = "https://github.com/CaseyCarter/cmcstl2/blob/master/LICENSE.txt"
    settings = ["os", "compiler", "arch"]
    generators = "cmake"
    exports_sources = ["CMakeLists.txt", "include/*", "test/*", "examples/*"]

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()
        cmake.test()
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        if self.settings.compiler == "gcc":
            self.cpp_info.cppflags = ["-fconcepts"]
        elif self.settings.compiler == "Visual Studio":
            self.cpp_info.cppflags = ["/std:c++latest"]
