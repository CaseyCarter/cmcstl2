// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stl2/detail/algorithm/equal.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../single_pass_array.hpp"

namespace ranges = __stl2;

int comparison_count = 0;

template<typename T>
bool counting_equals(const T &a, const T &b) {
	++comparison_count;
	return a == b;
}

int main() {
	using ranges::equal, ranges::distance, ranges::subrange;

	{
		static const int a[] = {0, 1, 2, 3, 4, 5};
		single_pass_array b {0, 1, 2, 3, 4, 5 };
		CHECK(equal(b, b));
		CHECK(equal(b, b, counting_equals<int>));
		CHECK(equal(a, b));
		CHECK(equal(b, a));
		CHECK(equal(begin(a), end(a), begin(b), end(b)));
		CHECK(equal(begin(b), end(b), begin(a), end(a)));
		CHECK(equal(begin(a), end(a), begin(b), end(b)));
		CHECK(equal(begin(b), end(b), begin(a), end(a)));
	}

	auto test_case = [](const bool result, const int count,
		const auto first1, const auto last1, const auto first2, const auto last2)
	{
		CHECK(equal(first1, last1, first2, last2) == result);

		comparison_count = 0;
		CHECK(equal(first1, last1, first2, last2, counting_equals<int>) == result);
		CHECK(comparison_count == count);

		CHECK(equal(subrange{first1, last1}, subrange{first2, last2}) == result);

		comparison_count = 0;
		CHECK(equal(subrange{first1, last1}, subrange{first2, last2}, counting_equals<int>) == result);
		CHECK(comparison_count == count);
	};

	using I = input_iterator<const int*>;
	using S = sentinel<const int*>;
	using R = random_access_iterator<const int*>;

	static const int ia[] = {0, 1, 2, 3, 4, 5};
	constexpr auto s = distance(ia);
	static const int ib[] = {0, 1, 2, 5, 4, 5};
	static_assert(distance(ib) == s);

	test_case(true,  s,     I(ia), S(ia + s), I(ia), S(ia + s));
	test_case(true,  s,     R(ia), R(ia + s), R(ia), R(ia + s));
	test_case(true,  s,     R(ia), S(ia + s), R(ia), S(ia + s));
	test_case(false, 4,     I(ia), S(ia + s), I(ib), S(ib + s));
	test_case(false, 4,     R(ia), R(ia + s), R(ib), R(ib + s));
	test_case(false, 4,     R(ia), S(ia + s), R(ib), S(ib + s));
	test_case(false, s - 1, I(ia), S(ia + s), I(ia), S(ia + s - 1));
	test_case(false, 0,     R(ia), R(ia + s), R(ia), R(ia + s - 1));
	test_case(false, s - 1, R(ia), S(ia + s), R(ia), S(ia + s - 1));

	return ::test_result();
}
