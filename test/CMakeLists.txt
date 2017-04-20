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
enable_testing()
include(CTest)

add_library(stl2_test_config INTERFACE)
target_link_libraries(stl2_test_config INTERFACE stl2)
target_compile_definitions(stl2_test_config INTERFACE $<$<CONFIG:Release>:NDEBUG>)
target_compile_options(stl2_test_config INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:
        -ftemplate-backtrace-limit=0 -Wall -Wextra -pedantic -Werror -march=native
        $<$<CONFIG:Debug>:-O0 -fno-inline -g3 -fstack-protector-all>
        $<$<CONFIG:Release>:-Ofast -g0>>)

function(add_stl2_test TESTNAME EXENAME SOURCES...)
  add_executable(${EXENAME} ${SOURCES...})
  target_link_libraries(${EXENAME} stl2_test_config)
  add_test(${TESTNAME} ${EXENAME})
endfunction(add_stl2_test)

function(add_simple_stl2_test NAME)
  add_stl2_test(test.${NAME} ${NAME} ${NAME}.cpp)
endfunction(add_simple_stl2_test)

add_simple_stl2_test(headers)
add_simple_stl2_test(range_access)
add_simple_stl2_test(tagged)
add_simple_stl2_test(common)
add_simple_stl2_test(variant)
add_simple_stl2_test(meta)
add_simple_stl2_test(optional)

add_subdirectory(concepts)
add_subdirectory(detail)
add_subdirectory(functional)
add_subdirectory(iterator)
add_subdirectory(algorithm)
add_subdirectory(view)
add_subdirectory(memory)
