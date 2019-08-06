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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stl2/detail/algorithm/is_permutation.hpp>
#include <stl2/utility.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace ranges = __stl2;

void test(const bool result,
	const int* first1, const int* last1,
	const int* first2, const int* last2)
{
	using ranges::is_permutation;
	using ranges::subrange;
	using I = forward_iterator<const int*>;
	using S = sentinel<const int*>;

	CHECK(is_permutation(I{first1}, I{last1}, I{first2}, I{last2}) == result);
	CHECK(is_permutation(I{first1}, S{last1}, I{first2}, S{last2}) == result);
	CHECK(is_permutation(I{first1}, I{last1}, I{first2}, I{last2}, std::equal_to<int>{}) == result);
	CHECK(is_permutation(I{first1}, S{last1}, I{first2}, S{last2}, std::equal_to<int>{}) == result);
	CHECK(is_permutation(subrange{I{first1}, I{last1}}, subrange{I{first2}, I{last2}}) == result);
	CHECK(is_permutation(subrange{I{first1}, S{last1}}, subrange{I{first2}, S{last2}}) == result);
	CHECK(is_permutation(subrange{I{first1}, I{last1}}, subrange{I{first2}, I{last2}}, std::equal_to<int>{}) == result);
	CHECK(is_permutation(subrange{I{first1}, S{last1}}, subrange{I{first2}, S{last2}}, std::equal_to<int>{}) == result);
}

int comparison_count = 0;

template<typename T>
bool counting_equals( T const &a, T const &b ) {
	++comparison_count;
	return a == b;
}

struct S {
	int i;
};

struct T {
	int i;
};

int main() {
	{
		const int ia[] = {0};
		const int ib[] = {0};
		test(true,  ia, ia + 0, ib, ib + 0);
		test(true,  ia, ia + 1, ib, ib + 1);
		test(false, ia, ia + 1, ib, ib + 0);
	}
	{
		const int ia[] = {0};
		const int ib[] = {1};
		test(false, ia, ia + 1, ib, ib + 1);
	}

	{
		const int ia[] = {0, 0};
		const int ib[] = {0, 0};
		test(true,  ia, ia + 2, ib, ib + 2);
		test(false, ia, ia + 2, ib, ib + 1);
	}
	{
		const int ia[] = {0, 0};
		const int ib[] = {0, 1};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {0, 0};
		const int ib[] = {1, 0};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {0, 0};
		const int ib[] = {1, 1};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {0, 1};
		const int ib[] = {0, 0};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {0, 1};
		const int ib[] = {0, 1};
		test(true,  ia, ia + 2, ib, ib + 2);
		test(false, ia, ia + 2, ib, ib + 1);
	}
	{
		const int ia[] = {0, 1};
		const int ib[] = {1, 0};
		test(true,  ia, ia + 2, ib, ib + 2);
		test(false, ia, ia + 2, ib, ib + 1);
	}
	{
		const int ia[] = {0, 1};
		const int ib[] = {1, 1};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {1, 0};
		const int ib[] = {0, 0};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {1, 0};
		const int ib[] = {0, 1};
		test(true,  ia, ia + 2, ib, ib + 2);
		test(false, ia, ia + 2, ib, ib + 1);
	}
	{
		const int ia[] = {1, 0};
		const int ib[] = {1, 0};
		test(true,  ia, ia + 2, ib, ib + 2);
		test(false, ia, ia + 2, ib, ib + 1);
	}
	{
		const int ia[] = {1, 0};
		const int ib[] = {1, 1};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {1, 1};
		const int ib[] = {0, 0};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {1, 1};
		const int ib[] = {0, 1};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {1, 1};
		const int ib[] = {1, 0};
		test(false, ia, ia + 2, ib, ib + 2);
	}
	{
		const int ia[] = {1, 1};
		const int ib[] = {1, 1};
		test(true,  ia, ia + 2, ib, ib + 2);
		test(false, ia, ia + 2, ib, ib + 1);
	}

	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 0, 0};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 0, 1};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 0, 2};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 1, 0};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 1, 1};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 1, 2};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 2, 0};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 2, 1};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 0};
		const int ib[] = {1, 2, 2};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 1};
		const int ib[] = {1, 0, 0};
		test(true,  ia, ia + 3, ib, ib + 3);
		test(false, ia, ia + 3, ib, ib + 2);
	}
	{
		const int ia[] = {0, 0, 1};
		const int ib[] = {1, 0, 1};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 1, 2};
		const int ib[] = {1, 0, 2};
		test(true,  ia, ia + 3, ib, ib + 3);
		test(false, ia, ia + 3, ib, ib + 2);
	}
	{
		const int ia[] = {0, 1, 2};
		const int ib[] = {1, 2, 0};
		test(true,  ia, ia + 3, ib, ib + 3);
		test(false, ia, ia + 3, ib, ib + 2);
	}
	{
		const int ia[] = {0, 1, 2};
		const int ib[] = {2, 1, 0};
		test(true,  ia, ia + 3, ib, ib + 3);
		test(false, ia, ia + 3, ib, ib + 2);
	}
	{
		const int ia[] = {0, 1, 2};
		const int ib[] = {2, 0, 1};
		test(true,  ia, ia + 3, ib, ib + 3);
		test(false, ia, ia + 3, ib, ib + 2);
	}
	{
		const int ia[] = {0, 0, 1};
		const int ib[] = {1, 0, 1};
		test(false, ia, ia + 3, ib, ib + 3);
	}
	{
		const int ia[] = {0, 0, 1};
		const int ib[] = {1, 0, 0};
		test(true,  ia, ia + 3, ib, ib + 3);
		test(false, ia, ia + 3, ib, ib + 2);
	}
	{
		const int ia[] = {0, 1, 2, 3, 0, 5, 6, 2, 4, 4};
		const int ib[] = {4, 2, 3, 0, 1, 4, 0, 5, 6, 2};
		constexpr auto sa = ranges::distance(ia);
		static_assert(sa == ranges::distance(ib));

		test(true,  ia, ia + sa, ib, ib + sa);
		test(false, ia, ia + sa, ib + 1, ib + sa);
		test(false, ia, ia + sa, ib, ib + sa - 1);

		comparison_count = 0;
		CHECK(ranges::is_permutation(
			forward_iterator(ia), forward_iterator(ia + sa),
			forward_iterator(ib), forward_iterator(ib + sa - 1),
			counting_equals<const int>) == false);
		CHECK(comparison_count > 0);

		comparison_count = 0;
		CHECK(ranges::is_permutation(
			random_access_iterator(ia), random_access_iterator(ia + sa),
			random_access_iterator(ib), random_access_iterator(ib + sa - 1),
			counting_equals<const int>) == false);
		CHECK(comparison_count == 0);
	}
	{
		const int ia[] = {0, 1, 2, 3, 0, 5, 6, 2, 4, 4};
		const int ib[] = {4, 2, 3, 0, 1, 4, 0, 5, 6, 0};
		constexpr auto sa = ranges::distance(ia);
		static_assert(sa == ranges::distance(ib));
		test(false, ia, ia + sa, ib, ib + sa);
	}

	{
		const S ia[] = {{0}, {1}, {2}, {3}, {0}, {5}, {6}, {2}, {4}, {4}};
		const T ib[] = {{4}, {2}, {3}, {0}, {1}, {4}, {0}, {5}, {6}, {2}};
		constexpr auto sa = ranges::distance(ia);
		static_assert(sa == ranges::distance(ib));

		// CommonView tests, with sentinels, with predicate and projections:
		CHECK(ranges::is_permutation(ia, ib, std::equal_to<int const>(), &S::i, &T::i) == true);
		CHECK(ranges::is_permutation(
			ranges::subrange(forward_iterator<const S*>(ia), sentinel<const S*>(ia + sa)),
			ranges::subrange(forward_iterator<const T*>(ib + 1), sentinel<const T*>(ib + sa)),
			std::equal_to<int const>(), &S::i, &T::i) == false);
		CHECK(ranges::is_permutation(
			ranges::subrange(forward_iterator<const S*>(ia), sentinel<const S*>(ia + sa)),
			ranges::subrange(forward_iterator<const T*>(ib), sentinel<const T*>(ib + sa - 1)),
			std::equal_to<int const>(), &S::i, &T::i) == false);
		CHECK(ranges::is_permutation(
			forward_iterator<const S*>(ia), sentinel<const S*>(ia + sa),
			forward_iterator<const T*>(ib), sentinel<const T*>(ib + sa),
			std::equal_to<int const>(), &S::i, &T::i) == true);

		// input_or_output_iterator tests, with sentinels, with predicate and projections:
		CHECK(ranges::is_permutation(
			forward_iterator<const S*>(ia), sentinel<const S*>(ia + sa),
			forward_iterator<const T*>(ib), sentinel<const T*>(ib + sa),
			std::equal_to<int const>(), &S::i, &T::i) == true);
		CHECK(ranges::is_permutation(
			forward_iterator<const S*>(ia), sentinel<const S*>(ia + sa),
			forward_iterator<const T*>(ib + 1), sentinel<const T*>(ib + sa),
			std::equal_to<int const>(), &S::i, &T::i) == false);
		CHECK(ranges::is_permutation(
			forward_iterator<const S*>(ia), sentinel<const S*>(ia + sa),
			forward_iterator<const T*>(ib), sentinel<const T*>(ib + sa - 1),
			std::equal_to<int const>(), &S::i, &T::i) == false);
	}

	{
		const int a[] = {0,1,2,3};
		const int b[] = {2,3,1,0};
		test(true, a, a + 4, b, b + 4);
	}

	return ::test_result();
}
