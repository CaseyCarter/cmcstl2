// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/algorithm/transform.hpp>

#include "../simple_test.hpp"

int main() {
	int rgi[]{1,2,3,4,5};
	__stl2::transform(rgi, rgi+5, rgi, [](int i){return i*2;});
	::check_equal(rgi, {2,4,6,8,10});

	__stl2::transform(rgi, rgi, [](int i){return i/2;});
	::check_equal(rgi, {1,2,3,4,5});

	return ::test_result();
}
