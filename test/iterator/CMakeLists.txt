add_executable(iterator iterator.cpp)
add_test(test.iterator.misc iterator)

add_executable(move_iterator move_iterator.cpp)
add_test(test.iterator.move move_iterator)

add_executable(reverse_iterator reverse_iterator.cpp)
add_test(test.iterator.reverse reverse_iterator)

add_executable(common_iterator common_iterator.cpp)
add_test(test.iterator.common common_iterator)

add_executable(counted_iterator counted_iterator.cpp)
add_test(test.iterator.counted counted_iterator)

add_executable(unreachable unreachable.cpp)
add_test(test.iterator.unreachable unreachable)

add_executable(istream_iterator istream_iterator.cpp)
add_test(test.iterator.istream istream_iterator)
