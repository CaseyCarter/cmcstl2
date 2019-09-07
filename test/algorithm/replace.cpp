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

#include <stl2/detail/algorithm/replace.hpp>
#include <stl2/detail/algorithm/equal.hpp>
#include <utility>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"
#include "../single_pass_array.hpp"

namespace ranges = __stl2;

template<typename Iter, typename Sent = Iter>
void test_iter()
{
	int ia[] = {0, 1, 2, 3, 4};
	const unsigned sa = sizeof(ia)/sizeof(ia[0]);
	Iter i = ranges::replace(Iter(ia), Sent(ia+sa), 2, 5);
	CHECK(ia[0] == 0);
	CHECK(ia[1] == 1);
	CHECK(ia[2] == 5);
	CHECK(ia[3] == 3);
	CHECK(ia[4] == 4);
	CHECK(base(i) == ia + sa);
}

template<typename Iter, typename Sent = Iter>
void test_rng()
{
	int ia[] = {0, 1, 2, 3, 4};
	const unsigned sa = sizeof(ia)/sizeof(ia[0]);
	auto rng = ranges::subrange(Iter(ia), Sent(ia+sa));
	Iter i = ranges::replace(rng, 2, 5);
	CHECK(ia[0] == 0);
	CHECK(ia[1] == 1);
	CHECK(ia[2] == 5);
	CHECK(ia[3] == 3);
	CHECK(ia[4] == 4);
	CHECK(base(i) == ia + sa);
}

int main()
{
	test_iter<input_iterator<int*> >();
	test_iter<forward_iterator<int*> >();
	test_iter<bidirectional_iterator<int*> >();
	test_iter<random_access_iterator<int*> >();
	test_iter<int*>();

	test_iter<input_iterator<int*>, sentinel<int*> >();
	test_iter<forward_iterator<int*>, sentinel<int*> >();
	test_iter<bidirectional_iterator<int*>, sentinel<int*> >();
	test_iter<random_access_iterator<int*>, sentinel<int*> >();

	test_rng<input_iterator<int*> >();
	test_rng<forward_iterator<int*> >();
	test_rng<bidirectional_iterator<int*> >();
	test_rng<random_access_iterator<int*> >();
	test_rng<int*>();

	test_rng<input_iterator<int*>, sentinel<int*> >();
	test_rng<forward_iterator<int*>, sentinel<int*> >();
	test_rng<bidirectional_iterator<int*>, sentinel<int*> >();
	test_rng<random_access_iterator<int*>, sentinel<int*> >();

	// test projection
	{
		using P = std::pair<int,std::string>;
		P ia[] = {{0,"0"}, {1,"1"}, {2,"2"}, {3,"3"}, {4,"4"}};
		P *i = ranges::replace(ia, 2, std::make_pair(42,"42"), &std::pair<int,std::string>::first);
		CHECK(ia[0] == P{0,"0"});
		CHECK(ia[1] == P{1,"1"});
		CHECK(ia[2] == P{42,"42"});
		CHECK(ia[3] == P{3,"3"});
		CHECK(ia[4] == P{4,"4"});
		CHECK(i == ranges::end(ia));
	}

	// test rvalue range
	{
		using P = std::pair<int,std::string>;
		P ia[] = {{0,"0"}, {1,"1"}, {2,"2"}, {3,"3"}, {4,"4"}};
		auto i = ranges::replace(std::move(ia), 2, std::make_pair(42,"42"), &std::pair<int,std::string>::first);
		static_assert(ranges::same_as<decltype(i), ranges::dangling>);
		CHECK(ia[0] == P{0,"0"});
		CHECK(ia[1] == P{1,"1"});
		CHECK(ia[2] == P{42,"42"});
		CHECK(ia[3] == P{3,"3"});
		CHECK(ia[4] == P{4,"4"});
	}

	// Check single pass
	{
		auto a = single_pass_array{1, 2, 3, 4};
		int expected[] = {1, 2, 3, 42};
		ranges::replace(a, 4, 42);
		CHECK(ranges::equal(a, expected));
	}
	{
		auto a = single_pass_array{1, 2, 3, 4};
		int expected[] = {1, 2, 3, 42};
		ranges::replace(ranges::begin(a), ranges::end(a), 4, 42);
		CHECK(ranges::equal(a, expected));
	}

	return ::test_result();
}
