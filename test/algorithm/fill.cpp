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

#include <stl2/detail/algorithm/fill.hpp>
#include <cstring>
#include <string>
#include <vector>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace stl2 = __stl2;

template <class Iter, class Sent = Iter>
void
test_char()
{
	const unsigned n = 4;
	char ca[n] = {0};
	auto i = stl2::fill(Iter(ca), Sent(ca+n), char(1));
	CHECK(ca[0] == 1);
	CHECK(ca[1] == 1);
	CHECK(ca[2] == 1);
	CHECK(ca[3] == 1);
	CHECK(i == Iter(ca + 4));

	auto rng = stl2::ext::subrange(Iter(ca), Sent(ca+n));
	i = stl2::fill(rng, char(2));
	CHECK(ca[0] == 2);
	CHECK(ca[1] == 2);
	CHECK(ca[2] == 2);
	CHECK(ca[3] == 2);
	CHECK(i == Iter(ca + 4));

	auto j = stl2::fill(stl2::ext::subrange(Iter(ca), Sent(ca+n)), char(3));
	CHECK(ca[0] == 3);
	CHECK(ca[1] == 3);
	CHECK(ca[2] == 3);
	CHECK(ca[3] == 3);
	CHECK(j.get_unsafe() == Iter(ca + 4));
}

template <class Iter, class Sent = Iter>
void
test_int()
{
	const unsigned n = 4;
	int ia[n] = {0};
	stl2::fill(Iter(ia), Sent(ia+n), 1);
	CHECK(ia[0] == 1);
	CHECK(ia[1] == 1);
	CHECK(ia[2] == 1);
	CHECK(ia[3] == 1);

	auto rng = stl2::ext::subrange(Iter(ia), Sent(ia+n));
	stl2::fill(rng, 2);
	CHECK(ia[0] == 2);
	CHECK(ia[2] == 2);
	CHECK(ia[2] == 2);
	CHECK(ia[3] == 2);
}

int main()
{
	test_char<forward_iterator<char*> >();
	test_char<bidirectional_iterator<char*> >();
	test_char<random_access_iterator<char*> >();
	test_char<char*>();

	test_char<forward_iterator<char*>, sentinel<char*> >();
	test_char<bidirectional_iterator<char*>, sentinel<char*> >();
	test_char<random_access_iterator<char*>, sentinel<char*> >();

	test_int<forward_iterator<int*> >();
	test_int<bidirectional_iterator<int*> >();
	test_int<random_access_iterator<int*> >();
	test_int<int*>();

	test_int<forward_iterator<int*>, sentinel<int*> >();
	test_int<bidirectional_iterator<int*>, sentinel<int*> >();
	test_int<random_access_iterator<int*>, sentinel<int*> >();

	return ::test_result();
}
