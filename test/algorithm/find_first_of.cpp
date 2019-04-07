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

#include <stl2/detail/algorithm/find_first_of.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"
#include "../single_pass_array.hpp"

namespace rng = std::experimental::ranges;

void test_iter()
{
	int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
	static constexpr unsigned sa = rng::size(ia);
	int ib[] = {1, 3, 5, 7};
	static constexpr unsigned sb = rng::size(ib);
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia + sa),
							 forward_iterator<const int*>(ib),
							 sentinel<const int*>(ib + sb)) ==
							 input_iterator<const int*>(ia+1));
	int ic[] = {7};
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia + sa),
							 forward_iterator<const int*>(ic),
							 sentinel<const int*>(ic + 1)) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia + sa),
							 forward_iterator<const int*>(ic),
							 sentinel<const int*>(ic)) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia),
							 forward_iterator<const int*>(ic),
							 sentinel<const int*>(ic+1)) ==
							 input_iterator<const int*>(ia));
}

void test_iter_pred()
{
	int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
	static constexpr unsigned sa = rng::size(ia);
	int ib[] = {1, 3, 5, 7};
	static constexpr unsigned sb = rng::size(ib);
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia + sa),
							 forward_iterator<const int*>(ib),
							 sentinel<const int*>(ib + sb),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia+1));
	int ic[] = {7};
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia + sa),
							 forward_iterator<const int*>(ic),
							 sentinel<const int*>(ic + 1),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia + sa),
							 forward_iterator<const int*>(ic),
							 sentinel<const int*>(ic),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(input_iterator<const int*>(ia),
							 sentinel<const int*>(ia),
							 forward_iterator<const int*>(ic),
							 sentinel<const int*>(ic+1),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia));
}

void test_rng()
{
	int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
	static constexpr unsigned sa = rng::size(ia);
	int ib[] = {1, 3, 5, 7};
	static constexpr unsigned sb = rng::size(ib);
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa))),
							 rng::subrange(forward_iterator<const int*>(ib),
							 forward_iterator<const int*>(ib + sb))) ==
							 input_iterator<const int*>(ia+1));
	CHECK(rng::find_first_of(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa)),
							 rng::subrange(forward_iterator<const int*>(ib),
							 forward_iterator<const int*>(ib + sb))) ==
							 input_iterator<const int*>(ia+1));
	int ic[] = {7};
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa))),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic + 1))) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa))),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic))) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia))),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic+1))) ==
							 input_iterator<const int*>(ia));
	CHECK(rng::find_first_of(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa)),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic + 1))) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa)),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic))) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia)),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic+1))) ==
							 input_iterator<const int*>(ia));
}

void test_single_pass() {
	using namespace ranges;
	auto a  = single_pass_array{0, 1, 2, 3, 0, 1, 2, 3};
	int ib[] = {3, 5, 7};
	{
		auto it = find_first_of(begin(a),
								end(a),
								forward_iterator<const int*>(ib),
								sentinel<const int*>(ib + 3)
								);
		CHECK((it != end(a) && *it == 3));
	}
	{
		auto it = find_first_of(a,
								subrange(forward_iterator<const int*>(ib),
								forward_iterator<const int*>(ib + 3))
								);
		CHECK((it != end(a) && *it == 3));
	}
}

void test_rng_pred()
{
	int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
	static constexpr unsigned sa = rng::size(ia);
	int ib[] = {1, 3, 5, 7};
	static constexpr unsigned sb = rng::size(ib);
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa))),
							 rng::subrange(forward_iterator<const int*>(ib),
							 forward_iterator<const int*>(ib + sb)),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia+1));
	int ic[] = {7};
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa))),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic + 1)),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia + sa))),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic)),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia+sa));
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const int*>(ia),
							 input_iterator<const int*>(ia))),
							 rng::subrange(forward_iterator<const int*>(ic),
							 forward_iterator<const int*>(ic+1)),
							 std::equal_to<int>()) ==
							 input_iterator<const int*>(ia));
}

struct S
{
	int i;
};

void test_rng_pred_proj()
{
	S ia[] = {S{0}, S{1}, S{2}, S{3}, S{0}, S{1}, S{2}, S{3}};
	static constexpr unsigned sa = rng::size(ia);
	S ib[] = {S{1}, S{3}, S{5}, S{7}};
	static constexpr unsigned sb = rng::size(ib);
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const S*>(ia),
							 input_iterator<const S*>(ia + sa))),
							 rng::subrange(forward_iterator<const S*>(ib),
							 forward_iterator<const S*>(ib + sb)),
							 std::equal_to<int>(), &S::i, &S::i) ==
							 input_iterator<const S*>(ia+1));
	S ic[] = {S{7}};
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const S*>(ia),
							 input_iterator<const S*>(ia + sa))),
							 rng::subrange(forward_iterator<const S*>(ic),
							 forward_iterator<const S*>(ic + 1)),
							 std::equal_to<int>(), &S::i, &S::i) ==
							 input_iterator<const S*>(ia+sa));
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const S*>(ia),
							 input_iterator<const S*>(ia + sa))),
							 rng::subrange(forward_iterator<const S*>(ic),
							 forward_iterator<const S*>(ic)),
							 std::equal_to<int>(), &S::i, &S::i) ==
							 input_iterator<const S*>(ia+sa));
	CHECK(rng::find_first_of(as_lvalue(rng::subrange(input_iterator<const S*>(ia),
							 input_iterator<const S*>(ia))),
							 rng::subrange(forward_iterator<const S*>(ic),
							 forward_iterator<const S*>(ic+1)),
							 std::equal_to<int>(), &S::i, &S::i) ==
							 input_iterator<const S*>(ia));
}

int main()
{
	::test_iter();
	::test_iter_pred();
	::test_rng();
	::test_single_pass();
	::test_rng_pred();
	::test_rng_pred_proj();
	return ::test_result();
}
