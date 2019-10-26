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
#include <stl2/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../single_pass_array.hpp"

namespace ranges = __stl2;
namespace views = ranges::views;

template <ranges::view v>
void f(v&&);

int main() {
	using ranges::view, ranges::sized_range, ranges::common_range;
	using ranges::input_range, ranges::forward_range, ranges::bidirectional_range, ranges::random_access_range;
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};
		auto x = views::counted(rg, 10);
		CHECK_EQUAL(x, {0,1,2,3,4,5,6,7,8,9});
		static_assert(view<decltype(x)>);
		static_assert(sized_range<decltype(x)>);
		static_assert(common_range<decltype(x)>);
		static_assert(random_access_range<decltype(x)>);
	}
	{
		int rg[] = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};
		auto x = views::counted(forward_iterator(rg), 10);
		CHECK_EQUAL(x, {0,1,2,3,4,5,6,7,8,9});
		static_assert(view<decltype(x)>);
		static_assert(sized_range<decltype(x)>);
		static_assert(!common_range<decltype(x)>);
		static_assert(forward_range<decltype(x)>);
		static_assert(!bidirectional_range<decltype(x)>);
	}

	{
		single_pass_array rg{0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9};
		auto x = views::counted(rg.begin(), 10);
		CHECK_EQUAL(x, {0,1,2,3,4,5,6,7,8,9});
		//f(x);
		//static_assert(view<decltype(x)>);
		static_assert(sized_range<decltype(x)>);
		static_assert(!common_range<decltype(x)>);
		static_assert(input_range<decltype(x)>);
		static_assert(!forward_range<decltype(x)>);
	}

	return test_result();
}
