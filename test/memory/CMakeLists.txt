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
add_stl2_test(memory.destroy destroy destroy.cpp)
add_stl2_test(memory.uninitialized_copy uninitialized_copy uninitialized_copy.cpp)
add_stl2_test(memory.uninitialized_default_construct uninitialized_default_construct uninitialized_default_construct.cpp)
add_stl2_test(memory.uninitialized_fill uninitialized_fill uninitialized_fill.cpp)
add_stl2_test(memory.uninitialized_move uninitialized_move uninitialized_move.cpp)
add_stl2_test(memory.uninitialized_value_construct uninitialized_value_construct uninitialized_value_construct.cpp)
