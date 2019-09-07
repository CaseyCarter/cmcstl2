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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stl2/detail/algorithm/minmax.hpp>
#include <cassert>
#include <memory>
#include <numeric>
#include <random>
#include <algorithm>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"
#include "../single_pass_array.hpp"

namespace ranges = __stl2;

namespace { std::mt19937 gen; }

template<class Iter, class Sent = Iter>
void test_iter(Iter first, Sent last) {
	assert(first != last);
	auto res = ranges::minmax(ranges::subrange(first, last));
	for (Iter i = first; i != last; ++i) {
		CHECK(!(*i < res.min));
		CHECK(!(res.max < *i));
	}
}

template<class Iter, class Sent = Iter>
void test_iter(unsigned N) {
	assert(N > 0);
	std::unique_ptr<int[]> a{new int[N]};
	std::iota(a.get(), a.get()+N, 0);
	std::shuffle(a.get(), a.get()+N, gen);
	test_iter(Iter(a.get()), Sent(a.get()+N));
}

template<class Iter, class Sent = Iter>
void test_iter() {
	test_iter<Iter, Sent>(1);
	test_iter<Iter, Sent>(2);
	test_iter<Iter, Sent>(3);
	test_iter<Iter, Sent>(10);
	test_iter<Iter, Sent>(1000);
}

template<class Iter, class Sent = Iter>
void test_iter_comp(Iter first, Sent last) {
	assert(first != last);
	typedef std::greater<int> Compare;
	Compare comp;
	auto res = ranges::minmax(ranges::subrange(first, last), comp);
	for (Iter i = first; i != last; ++i) {
		CHECK(!comp(*i, res.min));
		CHECK(!comp(res.max, *i));
	}
}

template<class Iter, class Sent = Iter>
void test_iter_comp(unsigned N) {
	assert(N > 0);
	std::unique_ptr<int[]> a{new int[N]};
	std::iota(a.get(), a.get()+N, 0);
	std::shuffle(a.get(), a.get()+N, gen);
	test_iter_comp(Iter(a.get()), Sent(a.get()+N));
}

template<class Iter, class Sent = Iter>
void test_iter_comp() {
	test_iter_comp<Iter, Sent>(1);
	test_iter_comp<Iter, Sent>(2);
	test_iter_comp<Iter, Sent>(3);
	test_iter_comp<Iter, Sent>(10);
	test_iter_comp<Iter, Sent>(1000);
}

struct S {
	int value;
	int index;
};

int main() {
	test_iter<input_iterator<const int*> >();
	test_iter<forward_iterator<const int*> >();
	test_iter<bidirectional_iterator<const int*> >();
	test_iter<random_access_iterator<const int*> >();
	test_iter<const int*>();
	test_iter<input_iterator<const int*>, sentinel<const int*>>();
	test_iter<forward_iterator<const int*>, sentinel<const int*>>();
	test_iter<bidirectional_iterator<const int*>, sentinel<const int*>>();
	test_iter<random_access_iterator<const int*>, sentinel<const int*>>();

	test_iter<input_iterator<const int*> >();
	test_iter<forward_iterator<const int*> >();
	test_iter<bidirectional_iterator<const int*> >();
	test_iter<random_access_iterator<const int*> >();
	test_iter<const int*>();
	test_iter<input_iterator<const int*>, sentinel<const int*>>();
	test_iter<forward_iterator<const int*>, sentinel<const int*>>();
	test_iter<bidirectional_iterator<const int*>, sentinel<const int*>>();
	test_iter<random_access_iterator<const int*>, sentinel<const int*>>();

	// Works with projections?
	S s[] = {S{1,0},S{2,1},S{3,2},S{4,3},S{-4,4},S{40,5},S{-4,6},S{40,7},S{7,8},S{8,9},S{9,10}};
	auto res = ranges::minmax(s, std::less<int>{}, &S::value);
	CHECK(res.min.value == -4);
	CHECK(res.min.index == 4);
	CHECK(res.max.value == 40);
	CHECK(res.max.index == 7);

	{
		auto a = single_pass_array{1, 2, 3, 4, 3, 2, 1};
		auto res = ranges::minmax(a);
		CHECK(res.min == 1);
		CHECK(res.max == 4);
	}

	return test_result();
}
