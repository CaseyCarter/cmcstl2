// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/algorithm/copy_n.hpp>
#include <algorithm>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

int main() {
	static const int source[] = {5,4,3,2,1,0};
	static constexpr std::ptrdiff_t n = sizeof(source)/sizeof(source[0]);
	int target[n];

	std::fill_n(target, n, 0);
	static_assert(n >= 2);
	auto res = stl2::copy_n(source, n - 2, target);
	CHECK(res.in == source + n - 2);
	CHECK(res.out == target + n - 2);

	CHECK(std::equal(source, source + n - 2, target));
	CHECK(target[n - 2] == 0);
	CHECK(target[n - 1] == 0);

	return test_result();
}
