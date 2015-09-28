# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Eric Niebler 2015
#  Copyright Casey Carter 2015
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#
# "Adapted" from Range-v3's top-level CMakeLists.txt
#
cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(CMCSTL2 CXX)

find_package(Git)

enable_testing()
include(CTest)
find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND-NOTFOUND)
  message("[W] Valgrind not found")
else()
  message("Valgrind: ${MEMORYCHECK_COMMAND}")
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
endif()

include_directories(include)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -march=native")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG")
endif()

add_subdirectory(test)
