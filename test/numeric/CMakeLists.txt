add_executable(numeric.accumulate accumulate.cpp)
add_test(test.numeric.accumulate numeric.accumulate)

add_executable(adjacent.difference adjacent_difference.cpp)
add_test(test.adjacent_difference adjacent.difference)

#add_executable(numeric.exclusive_scan exclusive_scan.cpp)
#add_test(test.numeric.exclusive_scan numeric.exclusive_scan)

add_executable(numeric.ops.gcd gcd.cpp)
add_test(test.numeric.ops.gcd numeric.ops.gcd)

#add_executable(numeric.inclusive_scan inclusive_scan.cpp)
#add_test(test.numeric.inclusive_scan numeric.inclusive_scan)

add_executable(inner.product inner_product.cpp)
target_compile_options(inner.product PRIVATE -Wno-deprecated-declarations)
add_test(test.inner.product inner.product)

add_executable(numeric.iota iota.cpp)
add_test(test.numeric.iota numeric.iota)

add_executable(numeric.ops.lcm lcm.cpp)
add_test(test.numeric.ops.lcm numeric.ops.lcm)

add_executable(partial.sum partial_sum.cpp)
add_test(test.partial.sum partial.sum)

# add_executable(numeric.reduce reduce.cpp)
# add_test(test.numeric.reduce numeric.reduce)

#add_executable(numeric.transform_exclusive_scan transform_exclusive_scan.cpp)
#add_test(test.numeric.transform_exclusive_scan numeric.transform_exclusive_scan)

#add_executable(numeric.transform_inclusive_scan transform_inclusive_scan.cpp)
#add_test(test.numeric.transform_inclusive_scan numeric.transform_inclusive_scan)

# add_executable(numeric.transform_reduce transform_reduce.cpp)
# add_test(test.transform_reduce numeric.transform_reduce)
