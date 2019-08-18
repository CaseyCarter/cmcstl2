// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/stride.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"
#include <iterator>
#include <list>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/filter.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/istream.hpp>
#include <stl2/view/reverse.hpp>
#include <sstream>
#include <vector>

namespace ranges {
	using namespace __stl2;
} // namespace ranges

namespace views {
	using namespace ranges::views;
	using namespace ranges::views::ext;
} // namespace views

int main()
{
	auto const v = []
	{
		auto in = views::iota(0, 50);
		return std::vector<int>{
			ranges::begin(in),
			ranges::end(in)
		};
	}();
	auto divisible_by_3 = [](auto const x) { return x % 3 == 0; };
	auto expected = views::iota(0, 50) | views::filter(divisible_by_3);

	{
		//auto r = ranges::stride_view{v, 3};
		auto r = v | views::stride(3);
		using R = decltype(r);
		static_assert(ranges::random_access_range<R>);
		static_assert(ranges::view<R>);
		static_assert(!ranges::contiguous_range<R>);
		static_assert(ranges::common_range<R>);
		static_assert(ranges::sized_range<R>);
		static_assert(ranges::range<R const>);

		CHECK_EQUAL(r, expected);
		CHECK_EQUAL(r | views::reverse, expected | views::reverse);
	}
	{
		auto str = std::stringstream{};
		ranges::copy(v, ranges::ostream_iterator<int>{str, " "});
		auto r = views::istream<int>(str) | views::stride(3);
		using R = decltype(r);
		static_assert(ranges::input_range<R>);
		static_assert(ranges::view<R>);
		static_assert(!ranges::forward_range<R>);
		static_assert(!ranges::common_range<R>);
		static_assert(!ranges::sized_range<R>);
		static_assert(!ranges::range<R const>);
		CHECK_EQUAL(r, {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
	}

	{
		// TODO: work out why ranges::begin(list)'s noexcept specifier "depends on itself"
		using I = decltype(bidirectional_iterator{ranges::begin(v)});
		auto li = ranges::subrange<I, I, ranges::subrange_kind::sized>{
			bidirectional_iterator{ranges::begin(v)},
			bidirectional_iterator{ranges::end(v)},
			static_cast<std::intmax_t>(ranges::size(v))
		};
		auto r = li | views::stride(3);
		using R = decltype(r);
		static_assert(ranges::bidirectional_range<R>);
		static_assert(ranges::view<R>);
		static_assert(!ranges::random_access_range<R>);
		static_assert(ranges::common_range<R>);
		static_assert(ranges::sized_range<R>);
		static_assert(ranges::range<R const>);

		auto expected = views::iota(0, 50) | views::filter(divisible_by_3);
		CHECK_EQUAL(r, expected);
		CHECK_EQUAL(r | views::reverse, r | views::reverse);
	}

	{
		auto x2 = v | views::stride(3);
		CHECK(ranges::distance(x2) == 17);
		
		auto it0 = x2.begin();
		auto it1 = ranges::next(it0, 10);
		CHECK((it1 - it0) == 10);
		CHECK((it0 - it1) == -10);
		CHECK((it0 - it0) == 0);
		CHECK((it1 - it1) == 0);
	}
#if 0 // TODO: enable when we have views::move
	{
		const auto n = 4;
		auto rng = v | view::move | views::stride(2);
		CHECK((ranges::next(ranges::begin(rng), n) - ranges::begin(rng)) == n);
	}
#endif // 0
	{
		// Regression test https://github.com/ericniebler/range-v3/issues/368
		int n = 42;
		(void)ranges::views::ext::stride(n);
	}
	{
		int const some_ints[] = {0,1,2,3,4,5,6,7};
		auto const some_input = ranges::subrange{
			input_iterator(ranges::begin(some_ints)),
			input_iterator(ranges::end(some_ints))
		};
		auto r = some_input | views::stride(2);
		using R = decltype(r);
		static_assert(ranges::input_range<R>);
		static_assert(!ranges::forward_range<R>);
		static_assert(!ranges::common_range<R>);
		static_assert(!ranges::sized_range<R>);
		static_assert(ranges::range<R const>);
		CHECK_EQUAL(r, {0,2,4,6});
	}
	{
		// TODO: revert to list once the bug has a workaround or a fix
		auto tmp = ranges::subrange{
			bidirectional_iterator(v.cbegin()),
			bidirectional_iterator(v.end())
		};
		auto r = tmp | views::stride(3);
		using R = decltype(r);
		static_assert(ranges::bidirectional_range<R>);
		static_assert(ranges::view<R>);
		static_assert(!ranges::random_access_range<R>);
		static_assert(!ranges::common_range<R>);
		static_assert(!ranges::sized_range<R>);
		static_assert(ranges::range<R const>);
		CHECK_EQUAL(r, expected);
		CHECK_EQUAL(r | views::reverse, r | views::reverse);
	}

	{
		// TODO: revert to list once the bug has a workaround or a fix
		using I = decltype(v.cbegin());
		auto tmp = ranges::subrange<I, I, ranges::subrange_kind::sized>{v};
		auto r = tmp | views::stride(3);
		using R = decltype(r);
		static_assert(ranges::random_access_range<R>);
		static_assert(ranges::view<R>);
		static_assert(!ranges::contiguous_range<R>);
		static_assert(ranges::common_range<R>);
		static_assert(ranges::sized_range<R>);
		static_assert(ranges::range<R const>);
		CHECK((*--r.end()) == 48);
		CHECK_EQUAL(r, expected);
		CHECK_EQUAL(r | views::reverse, expected | views::reverse);
	}


	// https://github.com/ericniebler/range-v3/issues/901
	{
		auto r = views::iota(0, 12);

		// Evenly divisible stride:
		auto strided1 = r | views::stride(3);
		CHECK_EQUAL(strided1, {0, 3, 6, 9});
		CHECK(strided1.size() == 4);
		CHECK(strided1.front() == 0);
		CHECK(strided1[0] == 0);
		CHECK(strided1.back() == 9);
		CHECK(strided1[3] == 9);
		CHECK(strided1[(int)strided1.size() - 1] == 9);

		// Not evenly divisible stride:
		auto strided2 = r | views::stride(5);
		CHECK_EQUAL(strided2, {0, 5, 10});
		CHECK(strided2.size() == 3);
		CHECK(strided2.front() == 0);
		CHECK(strided2[0] == 0);
		CHECK(strided2.back() == 10);
		CHECK(strided2[2] == 10);
		CHECK(strided2[(int)strided2.size() - 1] == 10);
	}

	return ::test_result();
}
