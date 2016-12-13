add_executable(headers headers.cpp)
add_test(test.headers headers)

add_executable(range_access range_access.cpp)
add_test(test.range_access range_access)

add_executable(tagged tagged.cpp)
add_test(test.tagged tagged)

add_executable(common common.cpp)
add_test(test.common common)

add_executable(variant variant.cpp)
add_test(test.variant variant)

add_executable(meta meta.cpp)
add_test(test.meta meta)

add_executable(optional optional.cpp)
add_test(test.optional optional)

add_subdirectory(concepts)
add_subdirectory(detail)
add_subdirectory(functional)
add_subdirectory(iterator)
add_subdirectory(algorithm)
add_subdirectory(view)
add_subdirectory(numeric)
add_subdirectory(memory)

