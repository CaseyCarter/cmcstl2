// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//

#include <stl2/view/zip.hpp>
#include <stl2/detail/algorithm/sort.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/functional/comparisons.hpp>
#include <forward_list>
#include <iostream>
#include <tuple>
#include <vector>
#include "../simple_test.hpp"

namespace ranges = __stl2;

int main() {
	using namespace ranges;

	{
		const int xs[] = {0,1,2,3};
		int ys[] = {4,5,6,7,8,9,10};
		auto rng = view::zip(xs, ys);
		CHECK(size(rng) == 4);
		CHECK_EQUAL(rng, {std::tuple<int, int>{0,4}, {1,5}, {2,6}, {3,7}});
	}

	{
		int xs[] = {3,2,1,0};
		int ys[] = {4,5,6,7,8,9,10};

		auto rng = view::zip(xs, ys);
		CHECK(size(rng) == 4);
		ranges::sort(rng, less{},
			[](auto&& x) { return std::get<0>(x); });

		CHECK_EQUAL(xs, {0,1,2,3});
		CHECK_EQUAL(ys, {7,6,5,4,8,9,10});
	}

	{
		auto rng = view::zip();
		using R = decltype(rng);
		static_assert(Same<value_type_t<iterator_t<R>>, std::tuple<>>);
		CHECK(empty(rng));
		ranges::sort(rng);
		CHECK_EQUAL(rng, std::initializer_list<std::tuple<>>{});
	}

	return test_result();
}
