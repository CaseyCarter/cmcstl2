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
#include "../single_pass_array.hpp"

namespace ranges = __stl2;

int main() {
	single_pass_array source = {5,4,3,2,1,0};
	static constexpr auto n = decltype(source)::size();
	int target[n]{};

	static_assert(n >= 2);
	auto res = ranges::copy_n(source.begin(), n - 2, target);
	CHECK(&*res.in == source.data() + n - 2);
	CHECK(res.out == target + n - 2);

	CHECK(std::equal(target, target + n - 2, source.data()));
	CHECK(target[n - 2] == 0);
	CHECK(target[n - 1] == 0);

	return test_result();
}
