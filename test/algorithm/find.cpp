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

struct S
{
	int i_;
};

template<class> class show_type;

int main()
{
	using namespace __stl2;

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

	S sa[] = {{0}, {1}, {2}, {3}, {4}, {5}};
	S *ps = find(sa, 3, &S::i_);
	CHECK((ps != end(sa) && ps->i_ == 3));
	ps = find(sa, 10, &S::i_);
	CHECK(ps == end(sa));

	return ::test_result();
}
