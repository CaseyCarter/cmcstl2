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
add_stl2_test(test.numeric.accumulate numeric.accumulate accumulate.cpp)
add_stl2_test(test.adjacent.difference adjacent.difference adjacent_difference.cpp)
#add_stl2_test(test.numeric.exclusive_scan numeric.exclusive_scan exclusive_scan.cpp)
add_stl2_test(test.numeric.ops.gcd numeric.ops.gcd gcd.cpp)
#add_stl2_test(test.numeric.inclusive_scan numeric.inclusive_scan inclusive_scan.cpp)
add_stl2_test(test.inner.product inner.product inner_product.cpp)
target_compile_options(inner.product PRIVATE -Wno-deprecated-declarations)
add_stl2_test(test.numeric.iota numeric.iota iota.cpp)
add_stl2_test(test.numeric.ops.lcm numeric.ops.lcm lcm.cpp)
add_stl2_test(test.partial.sum partial.sum partial_sum.cpp)
#add_stl2_test(test.numeric.reduce numeric.reduce reduce.cpp)
#add_stl2_test(test.numeric.transform_exclusive_scan numeric.transform_exclusive_scan transform_exclusive_scan.cpp)
#add_stl2_test(test.numeric.transform_inclusive_scan numeric.transform_inclusive_scan transform_inclusive_scan.cpp)
#add_stl2_test(test.numeric.transform_reduce numeric.transform_reduce transform_reduce.cpp)
