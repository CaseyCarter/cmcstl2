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

#include <stl2/detail/algorithm/generate.hpp>
#include <stl2/iterator.hpp>
#include <vector>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace ranges = __stl2;

struct gen_test {
	int i_;
	gen_test() = default;
	gen_test(int i) : i_(i) {}
	int operator()() {return i_++;}
};

template<class Iter, class Sent = Iter>
void test() {
	const unsigned n = 4;
	int ia[n] = {0};
	auto f = gen_test{1};
	auto res1 = ranges::generate(Iter(ia), Sent(ia + n), ranges::ref(f));
	CHECK(ia[0] == 1);
	CHECK(ia[1] == 2);
	CHECK(ia[2] == 3);
	CHECK(ia[3] == 4);
	CHECK(res1 == Iter(ia + n));
	CHECK(f.i_ == 5);

	auto rng = ranges::subrange(Iter(ia), Sent(ia + n));
	auto res2 = ranges::generate(rng, ranges::ref(f));
	CHECK(ia[0] == 5);
	CHECK(ia[1] == 6);
	CHECK(ia[2] == 7);
	CHECK(ia[3] == 8);
	CHECK(res2 == Iter(ia + n));
	CHECK(f.i_ == 9);

	auto res3 = ranges::generate(std::move(rng), ranges::ref(f));
	CHECK(ia[0] == 9);
	CHECK(ia[1] == 10);
	CHECK(ia[2] == 11);
	CHECK(ia[3] == 12);
	CHECK(res3 == Iter(ia + n));
	CHECK(f.i_ == 13);
}

void test2() {
	// Test ranges::generate with a genuine output range
	std::vector<int> v;
	auto rng = ranges::subrange(
		ranges::counted_iterator{ranges::back_inserter(v), 5},
		ranges::default_sentinel);
	ranges::generate(rng, gen_test(1));
	CHECK(v.size() == 5u);
	CHECK(v[0] == 1);
	CHECK(v[1] == 2);
	CHECK(v[2] == 3);
	CHECK(v[3] == 4);
	CHECK(v[4] == 5);
}

int main() {
	test<forward_iterator<int*> >();
	test<bidirectional_iterator<int*> >();
	test<random_access_iterator<int*> >();
	test<int*>();

	test<forward_iterator<int*>, sentinel<int*> >();
	test<bidirectional_iterator<int*>, sentinel<int*> >();
	test<random_access_iterator<int*>, sentinel<int*> >();

	test2();

	return ::test_result();
}
