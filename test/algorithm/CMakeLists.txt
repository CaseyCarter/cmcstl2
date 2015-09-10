include("../concept_select.txt")

add_executable(alg.all_of all_of.cpp)
add_test(test.alg.all_of alg.all_of)

add_executable(alg.any_of any_of.cpp)
add_test(test.alg.any_of alg.any_of)

add_executable(alg.transform transform.cpp)
add_test(test.alg.transform alg.transform)
