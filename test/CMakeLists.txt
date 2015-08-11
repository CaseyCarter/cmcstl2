add_executable(headers headers.cpp)
add_test(test.headers headers)

add_executable(tagged tagged.cpp)
add_test(test.tagged tagged)

add_subdirectory(concepts)
add_subdirectory(iterator)
add_subdirectory(algorithm)
