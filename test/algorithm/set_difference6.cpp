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

#include "set_difference.hpp"
#include <stl2/detail/algorithm/lexicographical_compare.hpp>

int main() {
	// Test projections
	{
		S ia[] = {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
		const int sa = sizeof(ia)/sizeof(ia[0]);
		T ib[] = {T{2}, T{4}, T{4}, T{6}};
		const int sb = sizeof(ib)/sizeof(ib[0]);
		U ic[20];
		int ir[] = {1, 2, 3, 3, 3, 4, 4};
		const int sr = sizeof(ir)/sizeof(ir[0]);

		ranges::set_difference_result<S*, U*> res =
			ranges::set_difference(ia, ib, ic, std::less<int>(), &S::i, &T::j);
		CHECK((res.in - ia) == sa);
		CHECK((res.out - ic) == sr);
		CHECK(!ranges::lexicographical_compare(ic, res.out, ir, ir+sr,
			std::less<int>(), &U::k));
		ranges::fill(ic, U{0});

		int irr[] = {6};
		const int srr = sizeof(irr)/sizeof(irr[0]);
		ranges::set_difference_result<T*, U*> res2 =
			ranges::set_difference(ib, ia, ic, std::less<int>(), &T::j, &S::i);
		CHECK((res2.in - ib) == sb);
		CHECK((res2.out - ic) == srr);
		CHECK(!ranges::lexicographical_compare(ic, res2.out, ir, irr+srr,
			std::less<int>(), &U::k));
	}

	// Test rvalue ranges
	{
		S ia[] = {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
		T ib[] = {T{2}, T{4}, T{4}, T{6}};
		U ic[20];
		int ir[] = {1, 2, 3, 3, 3, 4, 4};
		const int sr = sizeof(ir)/sizeof(ir[0]);

		auto res = ranges::set_difference(std::move(ia), std::move(ib), ic,
			std::less<int>(), &S::i, &T::j);
		static_assert(ranges::Same<decltype(res.in), ranges::dangling>);
		CHECK((res.out - ic) == sr);
		CHECK(!ranges::lexicographical_compare(ic, res.out, ir, ir+sr,
			std::less<int>(), &U::k));
		ranges::fill(ic, U{0});

		int irr[] = {6};
		const int srr = sizeof(irr)/sizeof(irr[0]);
		auto res2 = ranges::set_difference(std::move(ib), std::move(ia), ic,
			std::less<int>(), &T::j, &S::i);
		static_assert(ranges::Same<decltype(res2.in), ranges::dangling>);
		CHECK((res2.out - ic) == srr);
		CHECK(!ranges::lexicographical_compare(ic, res2.out, ir, irr+srr,
			std::less<int>(), &U::k));
	}

	return ::test_result();
}
