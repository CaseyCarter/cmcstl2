include("../concept_select.txt")

add_executable(alg.all_of all_of.cpp)
add_test(test.alg.all_of alg.all_of)

add_executable(alg.any_of any_of.cpp)
add_test(test.alg.any_of alg.any_of)

add_executable(alg.for_each for_each.cpp)
add_test(test.alg.for_each alg.for_each)

add_executable(alg.none_of none_of.cpp)
add_test(test.alg.none_of alg.none_of)

add_executable(alg.transform transform.cpp)
add_test(test.alg.transform alg.transform)
