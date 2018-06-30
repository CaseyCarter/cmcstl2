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

#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <algorithm>
#include <cstring>
#include "../simple_test.hpp"

namespace ranges = __stl2;

template <ranges::InputIterator I>
	requires ranges::Regular<ranges::value_type_t<I>>
class delimiter {
	ranges::value_type_t<I> value_;
public:
	delimiter() = default;
	delimiter(ranges::value_type_t<I> value) :
		value_{std::move(value)} {}

	friend bool operator==(const delimiter& lhs, const delimiter& rhs) {
		return lhs.value_ == rhs.value_;
	}
	friend bool operator!=(const delimiter& lhs, const delimiter& rhs) {
		return !(lhs == rhs);
	}
	friend bool operator==(const I& i, const delimiter& s) {
		return *i == s.value_;
	}
	friend bool operator==(const delimiter& s, const I& i) {
		return i == s;
	}
	friend bool operator!=(const I& i, const delimiter& s) {
		return !(i == s);
	}
	friend bool operator!=(const delimiter& s, const I& i) {
		return !(i == s);
	}
};

STL2_OPEN_NAMESPACE {
template <class I>
struct common_type<I, ::delimiter<I>> {
	using type = common_iterator<I, ::delimiter<I>>;
};
template <class I>
	struct common_type<::delimiter<I>, I> {
	using type = common_iterator<I, ::delimiter<I>>;
};
} STL2_CLOSE_NAMESPACE

int main()
{
	using ranges::begin;
	using ranges::end;
	using ranges::size;

	std::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
	static_assert(size(a) == 6, "");
	std::pair<int, int> out[size(a)] = {};

	auto res = ranges::copy(begin(a), end(a), out);
	CHECK(res.first == end(a));
	CHECK(res.second == out + size(out));
	CHECK(&res.first == &res.in());
	CHECK(&res.second == &res.out());
	CHECK(std::equal(a, a + size(a), out));

	std::fill_n(out, size(out), std::make_pair(0, 0));
	CHECK(!std::equal(a, a + size(a), out));

	res = ranges::copy(a, out);
	CHECK(res.first == a + size(a));
	CHECK(res.second == out + size(out));
	CHECK(std::equal(a, a + size(a), out));

	std::fill_n(out, size(out), std::make_pair(0, 0));

	{
		char const *sz = "hello world";
		char buf[50];
		auto str = ranges::ext::subrange(sz, delimiter<const char*>{'\0'});
		{
			std::fill_n(buf, sizeof(buf), '\0');
			auto res3 = ranges::copy(str, buf);
			*res3.second = '\0';
			CHECK(res3.first == std::next(begin(str), std::strlen(sz)));
			CHECK(res3.second == buf + std::strlen(sz));
			CHECK(std::strcmp(sz, buf) == 0);
		}
		{
			std::fill_n(buf, sizeof(buf), '\0');
			auto res4 = ranges::copy(std::move(str), buf);
			*res4.second = '\0';
			CHECK(res4.first == std::next(begin(str), std::strlen(sz)));
			CHECK(res4.second == buf + std::strlen(sz));
			CHECK(std::strcmp(sz, buf) == 0);
		}
	}

	{
		int target[8]{};
		auto l = {1,2,3,4,5,6};
		CHECK(ranges::copy(std::move(l), target + 1).out() == target + 7);
		CHECK_EQUAL(target, {0,1,2,3,4,5,6,0});
	}

	return test_result();
}
