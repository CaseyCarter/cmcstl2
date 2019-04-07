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

#include <stl2/detail/algorithm/mismatch.hpp>
#include <memory>
#include <algorithm>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"
#include "../single_pass_array.hpp"

STL2_OPEN_NAMESPACE {
	template<class I1, class I2>
	constexpr bool operator==(mismatch_result<I1, I2> const& x, mismatch_result<I1, I2> const& y) {
		return x.in1 == y.in1 && x.in2 == y.in2;
	}
} STL2_CLOSE_NAMESPACE

namespace ranges = __stl2;

template<typename Iter, typename Sent = Iter>
void test_range() {
	using S = ranges::subrange<Iter, Sent>;
	using R = ranges::mismatch_result<Iter, Iter>;

	auto test = [](S&& r1, S&& r2, const R& result) {
		using ranges::begin;
		using ranges::end;
		using ranges::mismatch;

		auto copy_of = [](const auto& x) { return x; };

		CHECK(mismatch(begin(r1), end(r1), begin(r2), end(r2)) == result);
		CHECK(mismatch(r1, r2) == result);
		CHECK(mismatch(r1, r2, std::equal_to<int>{}) == result);
		CHECK(mismatch(copy_of(r1), r2) == result);
		CHECK(mismatch(copy_of(r1), r2, std::equal_to<int>{}) == result);
		CHECK(mismatch(r1, copy_of(r2)) == result);
		CHECK(mismatch(r1, copy_of(r2), std::equal_to<int>{}) == result);
		CHECK(mismatch(copy_of(r1), copy_of(r2)) == result);
		CHECK(mismatch(copy_of(r1), copy_of(r2), std::equal_to<int>{}) == result);
	};

	const int ia[] = {0, 1, 2, 2, 0, 1, 2, 3};
	constexpr auto sa = ranges::distance(ia);
	const int ib[] = {0, 1, 2, 3, 0, 1, 2, 3};
	static_assert(sa == ranges::distance(ib));

	test(S{Iter(ia), Sent(ia + sa)}, S{Iter(ib), Sent(ib + sa)}, R{Iter(ia+3), Iter(ib+3)});
	test(S{Iter(ia), Sent(ia + sa)}, S{Iter(ib), Sent(ib + 2)}, R{Iter(ia+2), Iter(ib+2)});
}

struct S {
	int i;
};

int main() {
	test_range<input_iterator<const int*>>();
	test_range<forward_iterator<const int*>>();
	test_range<bidirectional_iterator<const int*>>();
	test_range<random_access_iterator<const int*>>();
	test_range<const int*>();
	test_range<input_iterator<const int*>, sentinel<const int*>>();
	test_range<forward_iterator<const int*>, sentinel<const int*>>();
	test_range<bidirectional_iterator<const int*>, sentinel<const int*>>();
	test_range<random_access_iterator<const int*>, sentinel<const int*>>();

	// Works with projections?
	S s1[] = {S{1}, S{2}, S{3}, S{4}, S{-4}, S{5}, S{6}, S{40}, S{7}, S{8}, S{9}};
	int const i1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	{
		ranges::mismatch_result<S const*, int const*> ps1
			= ranges::mismatch(s1, i1, std::equal_to<int>(), &S::i);
		CHECK(ps1.in1->i == -4);
		CHECK(*ps1.in2 == 5);
	}
	{
		ranges::mismatch_result<S const*, int const*> ps1
			= ranges::mismatch(ranges::begin(s1), ranges::end(s1),
				ranges::begin(i1), ranges::end(i1), std::equal_to<int>(), &S::i);
		CHECK(ps1.in1->i == -4);
		CHECK(*ps1.in2 == 5);
	}

	S s2[] = {S{1}, S{2}, S{3}, S{4}, S{5}, S{6}, S{40}, S{7}, S{8}, S{9}};
	{
		ranges::mismatch_result<S const*, S const*> ps2
			= ranges::mismatch(s1, s2, std::equal_to<int>(), &S::i, &S::i);
		CHECK(ps2.in1->i == -4);
		CHECK(ps2.in2->i == 5);
	}
	{
		ranges::mismatch_result<S const*, S const*> ps2
			= ranges::mismatch(ranges::begin(s1), ranges::end(s1),
				ranges::begin(s2), ranges::end(s2), std::equal_to<int>(), &S::i, &S::i);
		CHECK(ps2.in1->i == -4);
		CHECK(ps2.in2->i == 5);
	}

	{
		auto a = single_pass_array{1, 2, 3, 4, 5, 6, 7};
		auto b = single_pass_array{1, 2, 3, 4, 5, 5, 7};
		auto r = ranges::mismatch(ranges::begin(a), ranges::end(a), ranges::begin(b), ranges::end(b));
		CHECK(*(r.in1) == 6);
		CHECK(*(r.in2) == 5);
	}

	{
		auto a = single_pass_array{1, 2, 3, 4, 5, 6, 7};
		auto b = single_pass_array{1, 2, 3, 4, 5, 5, 7};
		auto r = ranges::mismatch(a, b);
		CHECK(*(r.in1) == 6);
		CHECK(*(r.in2) == 5);
	}

	return test_result();
}
