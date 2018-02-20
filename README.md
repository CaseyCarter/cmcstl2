# cmcstl2
An implementation of [ISO/IEC Technical Specification 21425:2017 "Programming languages -- C++ Extensions for ranges"](https://www.iso.org/standard/70910.html) (the "Ranges TS").
Compilation requires a compiler with support for C++17 and the Concepts TS, which as of this writing means [GCC 6+](https://gcc.gnu.org/) with the `-std=c++1z` and `-fconcepts` command line options.

## Build status
- on Travis-CI: [![Travis Build Status](https://travis-ci.org/CaseyCarter/cmcstl2.svg?branch=master)](https://travis-ci.org/CaseyCarter/cmcstl2)

## CMake Package

```cmake
find_package(cmcstl2 NO_MODULE REQUIRED)
target_link_libraries(app PRIVATE stl2)
```

## Conan Package
| | | |
|-|-|-|
| Bintray repository | `conan remote add mmha https://api.bintray.com/conan/mmha/conan` |
| Package reference  | `conan install cmcstl2/<commit_hash>@mmha/stable` |

### Usage
To use cmcstl2 in your CMake project, do the following:
```cmake
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                    "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()
    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_cmake_run(REQUIRES cmcstl2/<commit_hash>@mmha/stable
                    BASIC_SETUP CMAKE_TARGETS
                    BUILD missing)
    find_package(cmcstl2 REQUIRED)
    add_executable(app main.cpp)
    target_link_libraries(app PRIVATE stl2)
```

### Build it yourself
There is a conan recipe in this repository. To build the package by yourself, clone this repository and execute this:
```
conan create . johndoe/mychannel
```
