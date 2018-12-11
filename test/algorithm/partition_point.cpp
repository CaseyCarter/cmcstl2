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

#include <stl2/detail/algorithm/partition_point.hpp>
#include <stl2/iterator.hpp>
#include <stl2/view/iota.hpp>
#include <memory>
#include <utility>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace ranges = __stl2;

struct is_odd {
	constexpr bool operator()(int i) const noexcept {
		return i % 2 != 0;
	}
};

template<class Iter, class Sent = Iter>
void test_iter() {
	{
		const int ia[] = {2, 4, 6, 8, 10};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)), is_odd()) == Iter(ia));
	}
	{
		const int ia[] = {1, 2, 4, 6, 8};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)), is_odd()) == Iter(ia + 1));
	}
	{
		const int ia[] = {1, 3, 2, 4, 6};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)), is_odd()) == Iter(ia + 2));
	}
	{
		const int ia[] = {1, 3, 5, 2, 4, 6};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)), is_odd()) == Iter(ia + 3));
	}
	{
		const int ia[] = {1, 3, 5, 7, 2, 4};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)), is_odd()) == Iter(ia + 4));
	}
	{
		const int ia[] = {1, 3, 5, 7, 9, 2};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)), is_odd()) == Iter(ia + 5));
	}
	{
		const int ia[] = {1, 3, 5, 7, 9, 11};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)), is_odd()) == Iter(ia + 6));
	}
	{
		const int ia[] = {1, 3, 5, 2, 4, 6, 7};
		CHECK(ranges::partition_point(Iter(ranges::begin(ia)),
			Sent(ranges::begin(ia)), is_odd()) == Iter(ia));
	}
}

template<class Iter, class Sent = Iter>
void test_range() {
	{
		const int ia[] = {2, 4, 6, 8, 10};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)))), is_odd()) == Iter(ia));
	}
	{
		const int ia[] = {1, 2, 4, 6, 8};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)))), is_odd()) == Iter(ia + 1));
	}
	{
		const int ia[] = {1, 3, 2, 4, 6};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)))), is_odd()) == Iter(ia + 2));
	}
	{
		const int ia[] = {1, 3, 5, 2, 4, 6};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)))), is_odd()) == Iter(ia + 3));
	}
	{
		const int ia[] = {1, 3, 5, 7, 2, 4};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)))), is_odd()) == Iter(ia + 4));
	}
	{
		const int ia[] = {1, 3, 5, 7, 9, 2};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)))), is_odd()) == Iter(ia + 5));
	}
	{
		const int ia[] = {1, 3, 5, 7, 9, 11};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia)))), is_odd()) == Iter(ia + 6));
	}
	{
		const int ia[] = {1, 3, 5, 2, 4, 6, 7};
		CHECK(ranges::partition_point(::as_lvalue(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::begin(ia)))), is_odd()) == Iter(ia));
	}

	// An rvalue range
	{
		const int ia[] = {1, 3, 5, 7, 9, 2};
		CHECK(ranges::partition_point(ranges::subrange(Iter(ranges::begin(ia)),
			Sent(ranges::end(ia))), is_odd()) == Iter(ia + 5));
	}
}

template<ranges::Iterator I>
ranges::subrange<ranges::counted_iterator<I>, ranges::default_sentinel>
make_counted_view(I i, ranges::iter_difference_t<I> n) {
	return {ranges::counted_iterator{std::move(i), n}, {}};
}

template<class Iter>
void test_counted() {
	{
		const int ia[] = {2, 4, 6, 8, 10};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			ranges::distance(ia))), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia),
			ranges::distance(ia)));
	}
	{
		const int ia[] = {1, 2, 4, 6, 8};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			ranges::distance(ia))), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia + 1),
			ranges::distance(ia) - 1));
	}
	{
		const int ia[] = {1, 3, 2, 4, 6};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			ranges::distance(ia))), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia + 2),
			ranges::distance(ia) - 2));
	}
	{
		const int ia[] = {1, 3, 5, 2, 4, 6};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			ranges::distance(ia))), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia + 3),
			ranges::distance(ia) - 3));
	}
	{
		const int ia[] = {1, 3, 5, 7, 2, 4};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			ranges::distance(ia))), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia + 4),
			ranges::distance(ia) - 4));
	}
	{
		const int ia[] = {1, 3, 5, 7, 9, 2};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			ranges::distance(ia))), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia + 5),
			ranges::distance(ia) - 5));
	}
	{
		const int ia[] = {1, 3, 5, 7, 9, 11};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			ranges::distance(ia))), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia + 6),
			ranges::distance(ia) - 6));
	}
	{
		const int ia[] = {1, 3, 5, 2, 4, 6, 7};
		CHECK(ranges::partition_point(::as_lvalue(make_counted_view(Iter(ranges::begin(ia)),
			0)), is_odd()) == ranges::counted_iterator<Iter>(Iter(ia), 0));
	}
}

struct S {
	int i;
};

int main() {
	test_iter<forward_iterator<const int*> >();
	test_iter<forward_iterator<const int*>, sentinel<const int*>>();

	test_range<forward_iterator<const int*> >();
	test_range<forward_iterator<const int*>, sentinel<const int*>>();

	test_counted<forward_iterator<const int*> >();

	// Test projections
	const S ia[] = {S{1}, S{3}, S{5}, S{2}, S{4}, S{6}};
	CHECK(ranges::partition_point(ia, is_odd(), &S::i) == ia + 3);

	// Test infinite ranges
	CHECK(*ranges::partition_point(ranges::iota_view<int>{0},
		[](int i){ return i < 42; }) == 42);

	return ::test_result();
}
