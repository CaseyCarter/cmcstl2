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
cmake_minimum_required(VERSION 3.8)

project(cmcstl2 CXX)

option(BUILD_TESTS "Build the unit tests" OFF)
option(BUILD_EXAMPLES "Build the examples" OFF)

add_library(stl2 INTERFACE)
target_include_directories(stl2 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
target_compile_features(stl2 INTERFACE cxx_std_17)
target_compile_options(stl2 INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-fconcepts>)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS stl2 EXPORT cmcstl2-targets)
install(EXPORT cmcstl2-targets DESTINATION lib/cmake/cmcstl2)
file(
    WRITE ${PROJECT_BINARY_DIR}/cmcstl2-config.cmake
    "include(\${CMAKE_CURRENT_LIST_DIR}/cmcstl2-targets.cmake)")
install(
    FILES ${PROJECT_BINARY_DIR}/cmcstl2-config.cmake
    DESTINATION lib/cmake/cmcstl2)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    enable_testing()
    include(CTest)
    add_subdirectory(test)
endif()
