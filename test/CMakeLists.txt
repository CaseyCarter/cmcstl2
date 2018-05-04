# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Casey Carter 2015, 2017
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#

add_library(stl2_test_config INTERFACE)
target_link_libraries(stl2_test_config INTERFACE stl2)
target_compile_definitions(stl2_test_config INTERFACE $<$<CONFIG:Release>:NDEBUG>)
target_compile_options(stl2_test_config INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:
        -ftemplate-backtrace-limit=0 -Wall -Wextra -pedantic -Werror -march=native
        $<$<CONFIG:Debug>:-O0 -fno-inline -g3 -fstack-protector-all>
        $<$<CONFIG:Release>:-Ofast -g0>>)

function(add_stl2_test TESTNAME EXENAME FIRSTSOURCE)
  add_executable(${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(${EXENAME} stl2_test_config)
  add_test(${TESTNAME} ${EXENAME})
endfunction(add_stl2_test)

add_stl2_test(test.headers headers headers1.cpp headers2.cpp)
add_stl2_test(test.range_access range_access range_access.cpp)
add_stl2_test(test.tagged tagged tagged.cpp)
add_stl2_test(test.common common common.cpp)
add_stl2_test(test.meta meta meta.cpp)
add_stl2_test(test.optional optional optional.cpp)

add_subdirectory(concepts)
add_subdirectory(detail)
add_subdirectory(functional)
add_subdirectory(iterator)
add_subdirectory(algorithm)
add_subdirectory(view)
add_subdirectory(memory)
