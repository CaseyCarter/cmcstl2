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
#include <stl2/view/common.hpp>
#include <stl2/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

namespace ranges = __stl2;
namespace views = ranges::views;

int main() {
	using ranges::same_as;
	using ranges::view, ranges::sized_range, ranges::common_range;
	using ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range;
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
		auto x = rg | views::common;
		CHECK_EQUAL(x, {0,1,2,3,4,5,6,7,8,9});
		static_assert(view<decltype(x)>);
		static_assert(sized_range<decltype(x)>);
		static_assert(common_range<decltype(x)>);
		static_assert(random_access_range<decltype(x)>);
	}
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9};
		auto x = views::counted(bidirectional_iterator(rg), 5) | views::common;
		CHECK_EQUAL(x, {0,1,2,3,4});
		static_assert(view<decltype(x)>);
		static_assert(sized_range<decltype(x)>);
		static_assert(common_range<decltype(x)>);
		static_assert(forward_range<decltype(x)>);
		static_assert(!bidirectional_range<decltype(x)>);
		static_assert(same_as<decltype(x), decltype(views::common(x))>);
	}
	return test_result();
}
