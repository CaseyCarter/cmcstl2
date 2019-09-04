// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Eric Niebler
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/take_while.hpp>
#include <stl2/view/iota.hpp>
#include <list>
#include <vector>
#include "../simple_test.hpp"

namespace ranges = __stl2;
namespace views = ranges::views;

int main()
{
	auto rng0 = views::iota(10) | views::take_while([](int i) { return i != 25; });
	CHECK_EQUAL(rng0, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
	static_assert(ranges::view<decltype(rng0)>);
	static_assert(!ranges::common_range<decltype(rng0)>);
	static_assert(ranges::random_access_iterator<decltype(rng0.begin())>);

	std::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto rng1 = vi | views::take_while([](int i) { return i != 50; });
	static_assert(ranges::random_access_range<decltype(rng1)>);
	CHECK_EQUAL(rng1, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

#if 0 // DISABLED until generate is migrated to cmcstl2.
	{
		auto ns = views::generate([]() mutable {
			static int N;
			return ++N;
		});
		auto rng = ns | views::take_while([](int i) { return i < 5; });
		CHECK_EQUAL(rng, {1,2,3,4});
	}
#endif

	return ::test_result();
}
