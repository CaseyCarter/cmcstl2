# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Casey Carter 2015, 2017
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#
add_stl2_test(test.iterator.misc iterator iterator.cpp)
add_stl2_test(test.iterator.move move_iterator move_iterator.cpp)
add_stl2_test(test.iterator.reverse reverse_iterator reverse_iterator.cpp)
add_stl2_test(test.iterator.basic basic_iterator basic_iterator.cpp)
add_stl2_test(test.iterator.common common_iterator common_iterator.cpp)
add_stl2_test(test.iterator.counted counted_iterator counted_iterator.cpp)
add_stl2_test(test.iterator.unreachable unreachable unreachable.cpp)
add_stl2_test(test.iterator.istream istream_iterator istream_iterator.cpp)
add_stl2_test(test.iterator.ostream ostream_iterator ostream_iterator.cpp)
add_stl2_test(test.iterator.istreambuf istreambuf_iterator istreambuf_iterator.cpp)
add_stl2_test(test.iterator.ostreambuf ostreambuf_iterator ostreambuf_iterator.cpp)
add_stl2_test(test.iterator.make_range make_range make_range.cpp)
add_stl2_test(test.iterator.incomplete iter.incomplete incomplete.cpp)
add_stl2_test(test.iterator.operations iter.operations operations.cpp)
# any_iterator disabled (See https://github.com/CaseyCarter/cmcstl2/issues/63)
# add_stl2_test(test.iterator.any iter.any any_iterator.cpp)
