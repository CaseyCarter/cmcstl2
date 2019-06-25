# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Casey Carter
#  Copyright Christopher Di Bella
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#
include("../concept_select.txt")

add_stl2_test(test.numeric.accumulate numeric.accumulate accumulate.cpp)
add_stl2_test(test.numeric.partial_sum numeric.partial_sum partial_sum.cpp)
