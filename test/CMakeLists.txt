add_executable(headers headers.cpp)
add_test(test.headers headers)

add_executable(range_access range_access.cpp)
add_test(test.range_access range_access)

add_executable(tagged tagged.cpp)
add_test(test.tagged tagged)

add_executable(common common.cpp)
add_test(test.common common)

add_subdirectory(concepts)
add_subdirectory(iterator)
add_subdirectory(algorithm)
