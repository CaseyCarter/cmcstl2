// Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#if VALIDATE_RANGES
#include <range/v3/core.hpp>
#include <range/v3/algorithm/none_of.hpp>
#else
#include <stl2/detail/algorithm/none_of.hpp>
namespace ranges = __stl2;
#endif
#include <vector>
#include "../simple_test.hpp"

bool even(int n) { return n % 2 == 0; }

struct S {
	S(bool p) : test(p) { }

	bool p() const { return test; }

	bool test;
};

int main()
{
	std::vector<int> all_even { 0, 2, 4, 6 };
	std::vector<int> one_even { 1, 3, 4, 7 };
	std::vector<int> none_even { 1, 3, 5, 7 };
	CHECK(!ranges::none_of(all_even.begin(), all_even.end(), even));
	CHECK(!ranges::none_of(one_even.begin(), one_even.end(), even));
	CHECK(ranges::none_of(none_even.begin(), none_even.end(), even));

	CHECK(!ranges::none_of(all_even, even));
	CHECK(!ranges::none_of(one_even, even));
	CHECK(ranges::none_of(none_even, even));

	{
		auto il = {0, 2, 4, 6};
		CHECK(!ranges::none_of(std::move(il), [](int n) { return n % 2 == 0; }));
	}
	{
		auto il = {1, 3, 4, 7};
		CHECK(!ranges::none_of(std::move(il), [](int n) { return n % 2 == 0; }));
	}
	{
		auto il = {1, 3, 5, 7};
		CHECK(ranges::none_of(std::move(il), [](int n) { return n % 2 == 0; }));
	}

	std::vector<S> all_true { true, true, true };
	std::vector<S> one_true { false, false, true };
	std::vector<S> none_true { false, false, false };
	CHECK(!ranges::none_of(all_true.begin(), all_true.end(), &S::p));
	CHECK(!ranges::none_of(one_true.begin(), one_true.end(), &S::p));
	CHECK(ranges::none_of(none_true.begin(), none_true.end(), &S::p));

	CHECK(!ranges::none_of(all_true, &S::p));
	CHECK(!ranges::none_of(one_true, &S::p));
	CHECK(ranges::none_of(none_true, &S::p));

	{
		auto il = {S(true), S(true), S(true)};
		CHECK(!ranges::none_of(std::move(il), &S::p));
	}
	{
		auto il = {S(false), S(true), S(false)};
		CHECK(!ranges::none_of(std::move(il), &S::p));
	}
	{
		auto il = {S(false), S(false), S(false)};
		CHECK(ranges::none_of(std::move(il), &S::p));
	}

	return ::test_result();
}
