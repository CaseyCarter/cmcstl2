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
add_stl2_test(test.concepts.core concepts.core core.cpp)
add_stl2_test(test.concepts.compare concepts.compare compare.cpp)
add_stl2_test(test.concepts.fundamental concepts.fundamental fundamental.cpp)
add_stl2_test(test.concepts.iterator concepts.iterator iterator.cpp)
add_stl2_test(test.concepts.object concepts.object object.cpp)
add_stl2_test(test.concepts.range concepts.range range.cpp)
add_stl2_test(test.concepts.swap concepts.swap swap.cpp)
