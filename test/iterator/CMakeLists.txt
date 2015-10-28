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

add_executable(ostream_iterator ostream_iterator.cpp)
add_test(test.iterator.ostream ostream_iterator)

add_executable(istreambuf_iterator istreambuf_iterator.cpp)
add_test(test.iterator.istreambuf istreambuf_iterator)

add_executable(ostreambuf_iterator ostreambuf_iterator.cpp)
add_test(test.iterator.ostreambuf ostreambuf_iterator)

add_executable(make_range make_range.cpp)
add_test(test.iterator.make_range make_range)

add_executable(iter.incomplete incomplete.cpp)
add_test(test.iterator.incomplet iter.incomplete)
