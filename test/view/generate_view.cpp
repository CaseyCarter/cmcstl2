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
#include <stl2/view/generate.hpp>

#include "../move_only_string.hpp"
#include "../simple_test.hpp"
#include <stl2/view/take_exactly.hpp>

namespace ranges = __stl2;

namespace views {
	using namespace ranges::views;
	using namespace ext;
} // namespace views

int main()
{
	// Test for constant generator functions
	{
		int i = 0, j = 1;
		auto fib = views::generate([&]{ int tmp = i; i += j; std::swap(i, j); return tmp; });
		static_assert(ranges::input_range<decltype(fib)>);
		static_assert(ranges::view<decltype(fib)>);
		CHECK_EQUAL(fib | views::take_exactly(10), {0,1,1,2,3,5,8,13,21,34});
	}

	// Test for mutable-only generator functions
	{
		int i = 0, j = 1;
		auto fib = views::generate([=]()mutable->int{int tmp = i; i += j; std::swap(i, j); return tmp;});
		static_assert(ranges::input_range<decltype(fib)>);
		static_assert(ranges::view<decltype(fib)>);
		CHECK_EQUAL(fib | views::take_exactly(10), {0,1,1,2,3,5,8,13,21,34});
		// The generator cannot be called when it's const-qualified, so "fib const"
		// does not model view.
		static_assert(!ranges::view<decltype(fib) const>);
	}

#if 0 // unlike range-v3, cmcstl2 doesn't support move-only input views
	// Test for move-only generator functions
	{
		struct MoveOnlyFunction {
			move_only_string str_;
			int i_;

			char operator()()
			{ return str_.sz_[i_++]; }
		};
		auto r = views::generate(MoveOnlyFunction{"Hello, world!", 0}) | views::take_exactly(5);
		static_assert(ranges::input_range<decltype(r)>);
		static_assert(ranges::view<decltype(r)>);
		CHECK_EQUAL(r, {'H', 'e', 'l', 'l', 'o'});
	}
#endif // 0

	// Test for generator functions that return move-only types
	// https://github.com/ericniebler/range-v3/issues/905
	{
		using cmcstl2_test::move_only_string;

		char str[] = "gi";
		auto r = views::generate([&]{str[0]++; return move_only_string{str};}) | views::take_exactly(2);
		auto i = r.begin();
		CHECK(bool(*i == move_only_string{"hi"}));
		CHECK(bool(*i == move_only_string{"hi"}));
		CHECK(bool(*r.begin() == move_only_string{"hi"}));
		CHECK(bool(*r.begin() == move_only_string{"hi"}));
		static_assert(ranges::input_range<decltype(r)>);
		static_assert(ranges::view<decltype(r)>);
		CHECK_EQUAL(r, {move_only_string{"hi"}, move_only_string{"ii"}});
		static_assert(std::is_same<ranges::range_reference_t<decltype(r)>, move_only_string&&>::value, "");
	}

	// Test for generator functions that return internal references
	// https://github.com/ericniebler/range-v3/issues/807
	{
		int i = 42;
		auto r = views::generate([i]{return &i;});
		auto rng2 = std::move(r);
		auto it = rng2.begin();
		auto p = *it;
		auto p2 = *++it;
		CHECK(p == p2);
	}

	// Test that we only call the function once for each dereferenceable position
	// https://github.com/ericniebler/range-v3/issues/819
	{
		int i = 0;
		auto r = views::generate([&i]{return ++i;});
		auto rng2 = std::move(r);
		auto it = rng2.begin();
		CHECK(i == 0);
		CHECK(*it == 1);
		CHECK(i == 1);
		++it;
		CHECK(i == 1);
		CHECK(*it == 2);
		CHECK(i == 2);
	}

	return test_result();
}
