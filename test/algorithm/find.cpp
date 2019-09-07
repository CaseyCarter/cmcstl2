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

#include <stl2/detail/algorithm/find.hpp>
#include <stl2/utility.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../single_pass_array.hpp"

namespace ranges = __stl2;

struct S {
	int i_;
};

int main() {
	using ranges::find, ranges::size, ranges::subrange, ranges::end;

	int ia[] = {0, 1, 2, 3, 4, 5};
	auto first = [&ia]{ return input_iterator<const int*>{ia}; };
	constexpr auto s = size(ia);
	const auto last = sentinel<const int*>(ia + s);

	auto r = find(first(), last, 3);
	CHECK((r != last && *r == 3));

	r = find(first(), last, 10);
	CHECK(r == last);

	int *pi = find(ia, 3);
	CHECK((pi != ia+s && *pi == 3));
	pi = find(ia, 10);
	CHECK(pi == ia+s);

	auto pj = find(subrange(ia), 3);
	CHECK((pj != ia+s && *pj == 3));
	pj = find(subrange(ia), 10);
	CHECK(pj == ia+s);

	S sa[] = {{0}, {1}, {2}, {3}, {4}, {5}};
	S *ps = find(sa, 3, &S::i_);
	CHECK((ps != end(sa) && ps->i_ == 3));
	ps = find(sa, 10, &S::i_);
	CHECK(ps == end(sa));

	auto single_pass   = single_pass_array{0, 1, 2, 3};
	auto single_pass_r = find(single_pass, 2);
	CHECK((single_pass_r != end(single_pass) && *single_pass_r == 2));

	return ::test_result();
}
