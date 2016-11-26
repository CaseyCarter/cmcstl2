// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/move.hpp>
#include <stl2/detail/algorithm/count.hpp>
#include <stl2/detail/range/range.hpp>
#include <array>
#include <memory>
#include <vector>
#include "../simple_test.hpp"

namespace ranges = __stl2;

int main() {
	std::unique_ptr<int> some_ints[] = {
		std::make_unique<int>(0), std::make_unique<int>(1),
		std::make_unique<int>(2), std::make_unique<int>(3),
	};
	auto rng2 = ranges::move_view<decltype((some_ints))>{some_ints};
	CHECK(ranges::size(rng2) == ranges::size(some_ints));
	CHECK(!ranges::empty(rng2));
	auto last = ranges::end(rng2);
	auto first = ranges::begin(rng2);
	int count = 0;
	for (; first != last; ++first) {
		std::unique_ptr<int> tmp = *first;
		std::cout << *tmp << ' ';
		CHECK(count == *tmp);
		++count;
	}
	std::cout << '\n';
	CHECK(ranges::count(some_ints, std::unique_ptr<int>{}) == ranges::distance(some_ints));

	return test_result();
}
