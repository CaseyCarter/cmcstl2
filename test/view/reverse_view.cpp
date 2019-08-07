// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/reverse.hpp>
#include <stl2/view/counted.hpp>
#include <stl2/view/filter.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

namespace ranges = __stl2;
namespace views = ranges::views;

int main() {
	using ranges::view, ranges::range, ranges::sized_range, ranges::common_range;
	using ranges::bidirectional_range, ranges::random_access_range;
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
		auto x = rg | views::reverse;
		CHECK_EQUAL(x, {9,8,7,6,5,4,3,2,1,0});
		static_assert(view<decltype(x)>);
		static_assert(range<const decltype(x)>);
		static_assert(sized_range<decltype(x)>);
		static_assert(common_range<decltype(x)>);
		static_assert(random_access_range<decltype(x)>);
	}
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
		auto x = views::counted(bidirectional_iterator(rg), 5) | views::reverse;
		CHECK_EQUAL(x, {4,3,2,1,0});
		static_assert(view<decltype(x)>);
		static_assert(!range<const decltype(x)>);
		static_assert(sized_range<decltype(x)>);
		static_assert(common_range<decltype(x)>);
		static_assert(bidirectional_range<decltype(x)>);
		static_assert(!random_access_range<decltype(x)>);
	}
	{
		// Regression test for CaseyCarter/cmcstl2#223
		int a[] = {1, 7, 3, 6, 5, 2, 4, 8};
		auto r0 = views::reverse(a);
		auto is_even = [](int i) { return i % 2 == 0; };
		auto r1 = views::filter(r0, is_even);
		int sum = 0;
		for (auto i : r1) {
			sum += i;
		}
		CHECK(20 == sum);
	}

	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
#if 0
		auto x = rg | views::reverse | views::reverse;
		static_assert(same_as<decltype(x), ref_view<decltype(rg)>>);
		CHECK(&x.base() == &rg);
#else
		using ranges::_RangeImpl, ranges::viewable_range;
		auto x = views::reverse(rg);
		using R = decltype(x);
		static_assert(range<R>);
		static_assert(!_RangeImpl<R>);
		static_assert(view<R>);
		static_assert(viewable_range<R>);
#endif
	}
 	return test_result();
}
