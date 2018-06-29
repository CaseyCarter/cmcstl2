// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/object.hpp>
#include "../simple_test.hpp"

namespace ranges = __stl2;

using ranges::detail::raw_ptr;

struct A { int i; };
struct B : A { int j; };

int main() {
	using RA = raw_ptr<A>;
	using RB = raw_ptr<B>;

	static_assert(ranges::Regular<RA>);
	static_assert(ranges::StrictTotallyOrdered<RA>);
	static_assert(ranges::Regular<RB>);
	static_assert(ranges::StrictTotallyOrdered<RB>);

	CHECK(RA{} == RA{});
	CHECK(!(RA{} != RA{}));
	CHECK(RA{} == RB{});
	CHECK(!(RA{} != RB{}));

	RA ra{RB{}};
	CHECK(ra == nullptr);
	ra = RB{};
	CHECK(ra == nullptr);

	A a{42};
	ra = &a;
	CHECK(ra == &a);
	CHECK(ra->i == 42);
	B b;
	b.i = 13;
	b.j = 42;
	ra = &b;
	CHECK(ra == &b);
	CHECK(ra->i == 13);

	B array[4]{};
	RA r0 = &array[0];
	RA r1 = &array[1];
	CHECK(r0 != r1);
	CHECK(r0 < r1);
	CHECK(r0 <= r1);
	CHECK(r1 > r0);
	CHECK(r1 >= r0);

	CHECK(r0);
	CHECK(!!r1);
	CHECK((r0 ? true : false));

	return test_result();
}
