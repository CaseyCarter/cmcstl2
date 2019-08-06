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

int main() {
	using namespace ranges;
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
		auto x = rg | view::reverse;
		CHECK_EQUAL(x, {9,8,7,6,5,4,3,2,1,0});
		static_assert(View<decltype(x)>);
		static_assert(Range<const decltype(x)>);
		static_assert(SizedRange<decltype(x)>);
		static_assert(CommonRange<decltype(x)>);
		static_assert(RandomAccessRange<decltype(x)>);
	}
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
		auto x = view::counted(bidirectional_iterator(rg), 5) | view::reverse;
		CHECK_EQUAL(x, {4,3,2,1,0});
		static_assert(View<decltype(x)>);
		static_assert(!Range<const decltype(x)>);
		static_assert(SizedRange<decltype(x)>);
		static_assert(CommonRange<decltype(x)>);
		static_assert(BidirectionalRange<decltype(x)>);
		static_assert(!RandomAccessRange<decltype(x)>);
	}
	{
		// Regression test for CaseyCarter/cmcstl2#223
		int a[] = {1, 7, 3, 6, 5, 2, 4, 8};
		auto r0 = ranges::view::reverse(a);
		auto is_even = [](int i) { return i % 2 == 0; };
		auto r1 = ranges::view::filter(r0, is_even);
		int sum = 0;
		for (auto i : r1) {
			sum += i;
		}
		CHECK(20 == sum);
	}

	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
#if 0
		auto x = rg | view::reverse | view::reverse;
		static_assert(same_as<decltype(x), ref_view<decltype(rg)>>);
		CHECK(&x.base() == &rg);
#else
		auto x = view::reverse(rg);
		using R = decltype(x);
		static_assert(Range<R>);
		static_assert(!_RangeImpl<R>);
		static_assert(View<R>);
		static_assert(ViewableRange<R>);
#endif
	}
 	return test_result();
}
