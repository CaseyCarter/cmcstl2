add_subdirectory(concepts)

add_executable(iterator iterator.cpp)
add_test(test.iterator iterator)

add_executable(headers headers.cpp)
add_test(test.headers headers)
