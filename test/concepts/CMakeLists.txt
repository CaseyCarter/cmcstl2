include("../concept_select.txt")

add_executable(concepts.core core.cpp)
add_test(test.concepts.core concepts.core)

add_executable(concepts.compare compare.cpp)
add_test(test.concepts.compare concepts.compare)

add_executable(concepts.fundamental fundamental.cpp)
add_test(test.concepts.fundamental concepts.fundamental)

add_executable(concepts.iterator iterator.cpp)
add_test(test.concepts.iterator concepts.iterator)

add_executable(concepts.number number.cpp)
add_test(test.concepts.number concepts.number)

add_executable(concepts.object object.cpp)
add_test(test.concepts.object concepts.object)

add_executable(concepts.range range.cpp)
add_test(test.concepts.range concepts.range)

add_executable(concepts.swap swap.cpp)
add_test(test.concepts.swap concepts.swap)
