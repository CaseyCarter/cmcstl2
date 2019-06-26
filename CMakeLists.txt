# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Eric Niebler 2015
#  Copyright Casey Carter 2015, 2017
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#
cmake_minimum_required(VERSION 3.12)

project(cmcstl2 CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

# check if cmcstl2 is not a subproject
if(CMAKE_CURRENT_LIST_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(STL2_BUILD_EXAMPLES "Build stl2 examples" ON)
    option(STL2_BUILD_TESTING "Build stl2 tests" ON)
else()
    option(STL2_BUILD_EXAMPLES "Build stl2 examples" OFF)
    option(STL2_BUILD_TESTING "Build stl2 tests" OFF)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config/cmake")
find_package(Sanitizer COMPONENTS address undefined)

add_library(stl2 INTERFACE)
target_include_directories(stl2 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
target_compile_features(stl2 INTERFACE cxx_std_20)
target_compile_options(stl2 INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-fconcepts>
    $<$<CXX_COMPILER_ID:Clang>:-Xclang -fconcepts-ts>
    $<$<CXX_COMPILER_ID:MSVC>:/permissive- /experimental:concepts>)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS stl2 EXPORT cmcstl2-targets)
install(EXPORT cmcstl2-targets DESTINATION lib/cmake/cmcstl2)
file(
    WRITE ${PROJECT_BINARY_DIR}/cmcstl2-config.cmake
    "include(\${CMAKE_CURRENT_LIST_DIR}/cmcstl2-targets.cmake)")
install(
    FILES ${PROJECT_BINARY_DIR}/cmcstl2-config.cmake
    DESTINATION lib/cmake/cmcstl2)

if(STL2_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(STL2_BUILD_TESTING)
    include(CTest)
    add_custom_target(stl2-check ${CMAKE_CTEST_COMMAND} -V)
    add_subdirectory(test)
endif()
