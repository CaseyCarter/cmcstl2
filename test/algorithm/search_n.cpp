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

#include <stl2/detail/algorithm/search_n.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace stl2 = __stl2;

template <class Iter, typename Sent = Iter>
void
test_iter_impl()
{
	{
		int ia[] = {0, 1, 2, 3, 4, 5};
		const unsigned sa = sizeof(ia)/sizeof(ia[0]);
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 0, 0);
			auto expected = stl2::subrange(Iter(ia), Iter(ia));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
if (stl2::SizedSentinel<Iter, Sent>) {
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 1, 0);
			auto expected = stl2::subrange(Iter(ia+0), Iter(ia+1));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
}
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 2, 0);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.empty());
			CHECK(result.begin() == expected.begin());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), sa, 0);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 0, 3);
			auto expected = stl2::subrange(Iter(ia), Iter(ia));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 1, 3);
			auto expected = stl2::subrange(Iter(ia+3), Iter(ia+4));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 2, 3);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), sa, 3);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 0, 5);
			auto expected = stl2::subrange(Iter(ia), Iter(ia));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 1, 5);
			auto expected = stl2::subrange(Iter(ia+5), Iter(ia+6));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), 2, 5);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ia), Sent(ia+sa), sa, 5);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
	}

	{
		int ib[] = {0, 0, 1, 1, 2, 2};
		const unsigned sb = sizeof(ib)/sizeof(ib[0]);
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), count, 0);
			auto expected = stl2::subrange(Iter(ib), Iter(ib + count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), count, 0);
			auto expected = stl2::subrange(Iter(ib), Iter(ib + count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), count, 0);
			auto expected = stl2::subrange(Iter(ib), Iter(ib + count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 3, 0);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), sb, 0);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 0, 1);
			auto expected = stl2::subrange(Iter(ib), Iter(ib));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 1, 1);
			auto expected = stl2::subrange(Iter(ib+2), Iter(ib+3));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 2, 1);
			auto expected = stl2::subrange(Iter(ib+2), Iter(ib+4));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 3, 1);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), sb, 1);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 0, 2);
			auto expected = stl2::subrange(Iter(ib), Iter(ib));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 1, 2);
			auto expected = stl2::subrange(Iter(ib+4), Iter(ib+5));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 2, 2);
			auto expected = stl2::subrange(Iter(ib+4), Iter(ib+6));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), 3, 2);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ib), Sent(ib+sb), sb, 2);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
	}

	{
		int ic[] = {0, 0, 0};
		const unsigned sc = sizeof(ic)/sizeof(ic[0]);
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(Iter(ic), Sent(ic+sc), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(Iter(ic), Sent(ic+sc), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(Iter(ic), Sent(ic+sc), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 3;
			auto result = stl2::search_n(Iter(ic), Sent(ic+sc), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(Iter(ic), Sent(ic+sc), 4, 0);
			auto expected = stl2::subrange(Iter(ic+sc), Iter(ic+sc));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
	}
}

template <class Iter, class Iter2>
void
test_iter()
{
	using Sent = typename sentinel_type<Iter>::type;
	test_iter_impl<Iter>();
	test_iter_impl<Iter, Sent>();

	using SizedSent1 = typename sentinel_type<Iter, true>::type;
	test_iter_impl<Iter, SizedSent1>();
}

template <class Iter, typename Sent = Iter>
void
test_range_impl()
{
	{
		int ia[] = {0, 1, 2, 3, 4, 5};
		const unsigned sa = sizeof(ia)/sizeof(ia[0]);
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 0);
			auto expected = stl2::subrange(Iter(ia), Iter(ia+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 0);
			auto expected = stl2::subrange(Iter(ia), Iter(ia+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), 2, 0);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), sa, 0);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 3);
			auto expected = stl2::subrange(Iter(ia), Iter(ia+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 3);
			auto expected = stl2::subrange(Iter(ia+3), Iter(ia+3+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 3);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), sa, 3);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 5);
			auto expected = stl2::subrange(Iter(ia), Iter(ia+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 5);
			auto expected = stl2::subrange(Iter(ia+5), Iter(ia+5+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), count, 5);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ia), Sent(ia+sa))), sa, 5);
			auto expected = stl2::subrange(Iter(ia+sa), Iter(ia+sa));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
	}

	{
		int ib[] = {0, 0, 1, 1, 2, 2};
		constexpr unsigned sb = sizeof(ib)/sizeof(ib[0]);
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 0);
			auto expected = stl2::subrange(Iter(ib), Iter(ib+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 0);
			auto expected = stl2::subrange(Iter(ib), Iter(ib+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 0);
			auto expected = stl2::subrange(Iter(ib), Iter(ib+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 3;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 0);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = sb;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 0);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 1);
			auto expected = stl2::subrange(Iter(ib), Iter(ib+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 1);
			auto expected = stl2::subrange(Iter(ib+2), Iter(ib+2+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 1);
			auto expected = stl2::subrange(Iter(ib+2), Iter(ib+2+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 3;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 1);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = sb;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 1);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 2);
			auto expected = stl2::subrange(Iter(ib), Iter(ib));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 2);
			auto expected = stl2::subrange(Iter(ib+4), Iter(ib+4+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 2);
			auto expected = stl2::subrange(Iter(ib+4), Iter(ib+4+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 3;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 2);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = sb;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ib), Sent(ib+sb))), count, 2);
			auto expected = stl2::subrange(Iter(ib+sb), Iter(ib+sb));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
	}

	{
		int ic[] = {0, 0, 0};
		const unsigned sc = sizeof(ic)/sizeof(ic[0]);
		{
			constexpr auto count = 0;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ic), Sent(ic+sc))), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 1;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ic), Sent(ic+sc))), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 2;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ic), Sent(ic+sc))), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 3;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ic), Sent(ic+sc))), count, 0);
			auto expected = stl2::subrange(Iter(ic), Iter(ic+count));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
		{
			constexpr auto count = 4;
			auto result = stl2::search_n(::as_lvalue(stl2::subrange(Iter(ic), Sent(ic+sc))), count, 0);
			auto expected = stl2::subrange(Iter(ic+sc), Iter(ic+sc));
			CHECK(result.begin() == expected.begin());
			CHECK(result.end() == expected.end());
		}
	}
}

template <class Iter, class Iter2>
void
test_range()
{
	using Sent = typename sentinel_type<Iter>::type;
	test_range_impl<Iter>();
	test_range_impl<Iter, Sent>();

	using SizedSent1 = typename sentinel_type<Iter, true>::type;
	test_range_impl<Iter, SizedSent1>();
}

template <class Iter, class Iter2>
void
test()
{
	test_iter<Iter, Iter2>();
	test_range<Iter, Iter2>();
}

struct S
{
	int i;
};

int main()
{
	test<forward_iterator<const int*>, forward_iterator<const int*> >();
	test<forward_iterator<const int*>, bidirectional_iterator<const int*> >();
	test<forward_iterator<const int*>, random_access_iterator<const int*> >();
	test<bidirectional_iterator<const int*>, forward_iterator<const int*> >();
	test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*> >();
	test<bidirectional_iterator<const int*>, random_access_iterator<const int*> >();
	test<random_access_iterator<const int*>, forward_iterator<const int*> >();
	test<random_access_iterator<const int*>, bidirectional_iterator<const int*> >();
	test<random_access_iterator<const int*>, random_access_iterator<const int*> >();

	// Test projections:
	{
		S const in[] = {{0}, {1}, {2}, {2}, {4}, {5}};

		auto result = stl2::search_n(in, 2, 2, stl2::equal_to<int>{}, &S::i);
		auto expected = stl2::subrange(in+2, in+4);
		CHECK(result.begin() == expected.begin());
		CHECK(result.end() == expected.end());
	}

	// Test counted ranges
	{
		// int in[] = {0,1,2,2,4,5};
		// auto rng = stl2::subrange(
		// 			 stl2::make_counted_iterator(bidirectional_iterator<int*>(in), 6),
		// 			 stl2::default_sentinel{});
		// auto result = stl2::search_n(rng, 2, 2);
		// auto expected = stl2::subrange();
		// CHECK(base(it.base()) == in+2);
		// CHECK(it.count() == 4);

		// auto it2 = stl2::search_n(rng, 3, 2);
		// CHECK(base(it2.base()) == in+6);
		// CHECK(it2.count() == 0);
	}

	// Test rvalue ranges
	{
		// int ib[] = {0, 0, 1, 1, 2, 2};
		// CHECK(stl2::search_n(stl2::move(ib), 2, 1).get_unsafe() == ib+2);
	}

	return ::test_result();
}
