// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <stl2/detail/algorithm/copy_backward.hpp>
#include <stl2/view/repeat.hpp>
#include <cstring>
#include <utility>
#include <algorithm>
#include "../simple_test.hpp"

namespace ranges = __stl2;

namespace {
	void test_repeat_view() {
		{
			auto v = ranges::ext::repeat_view<int>(42);
			int target[8]{};
			auto result = ranges::copy_backward(ranges::make_counted_iterator(v.begin(), 4),
				ranges::make_counted_iterator(v.begin(), 0), ranges::end(target));
			CHECK(result.in().count() == 0);
			CHECK(result.in().base() == v.begin());
			CHECK(result.out() == target + 4);
			CHECK(std::count(target, target + 4, 0) == 4);
			CHECK(std::count(target + 4, target + 8, 42) == 4);
		}
		{
			auto v = ranges::ext::repeat_view<int>(42);
			int target[8]{};
			auto result = ranges::copy_backward(ranges::make_counted_iterator(v.begin(), 4),
				ranges::default_sentinel{}, ranges::end(target));
			CHECK(result.in().count() == 0);
			CHECK(result.in().base() == v.begin());
			CHECK(result.out() == target + 4);
			CHECK(std::count(target, target + 4, 0) == 4);
			CHECK(std::count(target + 4, target + 8, 42) == 4);
		}
	}

	void test_initializer_list() {
		int target[8]{};
		auto result = ranges::copy_backward({1, 2, 3, 4}, ranges::end(target));
		CHECK(result.out() == target + 4);
		CHECK(std::count(target, target + 4, 0) == 4);
		check_equal(ranges::ext::make_range(target + 4, target + 8), {1, 2, 3, 4});
	}
}

int main()
{
	using ranges::begin;
	using ranges::end;
	using ranges::size;

	std::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
	static_assert(size(a) == 6, "");
	std::pair<int, int> out[size(a)] = {};

	auto res = ranges::copy_backward(begin(a), end(a), end(out));
	CHECK(res.first == end(a));
	CHECK(res.second == begin(out));
	CHECK(std::equal(a, a + size(a), out));

	std::fill_n(out, size(out), std::make_pair(0, 0));
	CHECK(!std::equal(a, a + size(a), out));

	res = ranges::copy_backward(a, end(out));
	CHECK(res.first == end(a));
	CHECK(res.second == begin(out));
	CHECK(std::equal(a, a + size(a), out));

	std::fill_n(out, size(out), std::make_pair(0, 0));
	auto res2 = ranges::copy_backward(ranges::move(a), end(out));
	CHECK(res2.first.get_unsafe() == end(a));
	CHECK(res2.second == begin(out));
	CHECK(std::equal(a, a + size(a), out));

	test_repeat_view();
	test_initializer_list();

	return test_result();
}
