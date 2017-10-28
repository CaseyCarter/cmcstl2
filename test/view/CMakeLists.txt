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
add_stl2_test(view.repeat view.repeat repeat_view.cpp)
add_stl2_test(view.repeat_n view.repeat_n repeat_n_view.cpp)
add_stl2_test(view.move view.move move_view.cpp)
add_stl2_test(view.filter view.filter filter_view.cpp)
add_stl2_test(view.transform view.transform transform_view.cpp)
add_stl2_test(view.take view.take take_view.cpp)
add_stl2_test(view.join view.join join_view.cpp)
add_stl2_test(view.split view.split split_view.cpp)
add_stl2_test(view.counted view.counted counted_view.cpp)
