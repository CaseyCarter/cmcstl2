# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Casey Carter 2015 - 2018
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#
add_stl2_test(span span span.cpp)
add_stl2_test(view.common view.common common_view.cpp)
add_stl2_test(view.counted view.counted counted_view.cpp)
add_stl2_test(view.drop view.drop drop_view.cpp)
add_stl2_test(view.drop_while view.drop_while drop_while_view.cpp)
add_stl2_test(view.empty view.empty empty_view.cpp)
add_stl2_test(view.filter view.filter filter_view.cpp)
add_stl2_test(view.generate view.generate generate_view.cpp)
add_stl2_test(view.indirect view.indirect indirect_view.cpp)
add_stl2_test(view.join view.join join_view.cpp)
add_stl2_test(view.move view.move move_view.cpp)
add_stl2_test(view.ref view.ref ref_view.cpp)
add_stl2_test(view.repeat view.repeat repeat_view.cpp)
add_stl2_test(view.repeat_n view.repeat_n repeat_n_view.cpp)
add_stl2_test(view.reverse view.reverse reverse_view.cpp)
add_stl2_test(view.single view.single single_view.cpp)
add_stl2_test(view.split view.split split_view.cpp)
add_stl2_test(view.subrange view.subrange subrange.cpp)
add_stl2_test(view.take view.take take_view.cpp)
add_stl2_test(view.take_exactly view.take_exactly take_exactly_view.cpp)
add_stl2_test(view.take_while view.take_while take_while_view.cpp)
add_stl2_test(view.transform view.transform transform_view.cpp)
