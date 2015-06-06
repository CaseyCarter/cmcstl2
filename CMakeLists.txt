cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(CMC-STL2 CXX)

# find_package(Doxygen)
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

include_directories(include meta/include)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -march=native -mtune=native")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -march=native -mtune=native")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG")
# else()
#   message(FATAL_ERROR "Unknown compiler.")
endif()


# add_subdirectory(doc)

# if(BIICODE)
#     ADD_BIICODE_TARGETS()

#     string(REPLACE " " ";" TARGET_CXX_FLAGS         ${CMAKE_CXX_FLAGS})
#     string(REPLACE " " ";" TARGET_CXX_FLAGS_DEBUG   ${CMAKE_CXX_FLAGS_DEBUG})
#     string(REPLACE " " ";" TARGET_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

#     target_compile_options(${BII_BLOCK_TARGET} INTERFACE ${TARGET_CXX_FLAGS}
#                            $<$<CONFIG:Debug>:${TARGET_CXX_FLAGS_DEBUG}>
#                            $<$<CONFIG:Release>:${TARGET_CXX_FLAGS_RELEASE}>)

#     return()
# endif()

add_subdirectory(test)
# add_subdirectory(example)
# add_subdirectory(perf)
# add_subdirectory(libs/range/v3/scratch)
