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

#include <stl2/detail/algorithm/search.hpp>
#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace ranges = __stl2;
using ranges::subrange;

template<class I>
constexpr bool eq(subrange<I> const& x, subrange<I> const& y) {
	return x.begin() == y.begin() && x.end() == y.end();
}

template<class Iter1, class Iter2, typename Sent1 = Iter1, typename Sent2 = Iter2>
void
test_iter_impl()
{
	int ia[] = {0, 1, 2, 3, 4, 5};
	const unsigned sa = sizeof(ia)/sizeof(ia[0]);

	CHECK(eq(subrange{Iter1(ia), Iter1(ia)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia), Sent2(ia))));
	CHECK(eq(subrange{Iter1(ia), Iter1(ia + 1)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia), Sent2(ia+1))));
	CHECK(eq(subrange{Iter1(ia + 1), Iter1(ia+2)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia+1), Sent2(ia+2))));
	CHECK(eq(subrange{Iter1(ia), Iter1(ia)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia+2), Sent2(ia+2))));
	CHECK(eq(subrange{Iter1(ia+2), Iter1(ia+3)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia+2), Sent2(ia+3))));
	CHECK(eq(subrange{Iter1(ia), Iter1(ia)},
		ranges::search(Iter1(ia), Sent1(ia), Iter2(ia+2), Sent2(ia+3))));
	CHECK(eq(subrange{Iter1(ia+sa-1), Iter1(ia+sa)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia+sa-1), Sent2(ia+sa))));
	CHECK(eq(subrange{Iter1(ia+sa-3), Iter1(ia+sa)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia+sa-3), Sent2(ia+sa))));
	CHECK(eq(subrange{Iter1(ia), Iter1(ia+sa)},
		ranges::search(Iter1(ia), Sent1(ia+sa), Iter2(ia), Sent2(ia+sa))));
	CHECK(eq(subrange{Iter1(ia+sa-1), Iter1(ia+sa-1)},
		ranges::search(Iter1(ia), Sent1(ia+sa-1), Iter2(ia), Sent2(ia+sa))));
	CHECK(eq(subrange{Iter1(ia+1), Iter1(ia+1)},
		ranges::search(Iter1(ia), Sent1(ia+1), Iter2(ia), Sent2(ia+sa))));

	int ib[] = {0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 4};
	const unsigned sb = sizeof(ib)/sizeof(ib[0]);
	int ic[] = {1};
	CHECK(eq(subrange{Iter1(ib+1), Iter1(ib+2)},
		ranges::search(Iter1(ib), Sent1(ib+sb), Iter2(ic), Sent2(ic+1))));
	int id[] = {1, 2};
	CHECK(eq(subrange{Iter1(ib+1), Iter1(ib+3)},
		ranges::search(Iter1(ib), Sent1(ib+sb), Iter2(id), Sent2(id+2))));
	int ie[] = {1, 2, 3};
	CHECK(eq(subrange{Iter1(ib+4), Iter1(ib+7)},
		ranges::search(Iter1(ib), Sent1(ib+sb), Iter2(ie), Sent2(ie+3))));
	int ig[] = {1, 2, 3, 4};
	CHECK(eq(subrange{Iter1(ib+8), Iter1(ib+12)},
		ranges::search(Iter1(ib), Sent1(ib+sb), Iter2(ig), Sent2(ig+4))));

	int ih[] = {0, 1, 1, 1, 1, 2, 3, 0, 1, 2, 3, 4};
	const unsigned sh = sizeof(ih)/sizeof(ih[0]);
	int ii[] = {1, 1, 2};
	CHECK(eq(subrange{Iter1(ih+3), Iter1(ih+6)},
		ranges::search(Iter1(ih), Sent1(ih+sh), Iter2(ii), Sent2(ii+3))));

	int ij[] = {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
	const unsigned sj = sizeof(ij)/sizeof(ij[0]);
	int ik[] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
	const unsigned sk = sizeof(ik)/sizeof(ik[0]);
	CHECK(eq(subrange{Iter1(ij+6), Iter1(ij+6+sk)},
		ranges::search(Iter1(ij), Sent1(ij+sj), Iter2(ik), Sent2(ik+sk))));
}

template<class Iter1, class Iter2>
void
test_iter()
{
	using Sent1 = typename sentinel_type<Iter1>::type;
	using Sent2 = typename sentinel_type<Iter2>::type;
	test_iter_impl<Iter1, Iter2>();
	test_iter_impl<Iter1, Iter2, Sent1>();
	test_iter_impl<Iter1, Iter2, Iter1, Sent2>();
	test_iter_impl<Iter1, Iter2, Sent1, Sent2>();

	using SizedSent1 = typename sentinel_type<Iter1, true>::type;
	using SizedSent2 = typename sentinel_type<Iter2, true>::type;
	test_iter_impl<Iter1, Iter2, SizedSent1, SizedSent2>();
}

template<class Iter1, class Iter2, typename Sent1 = Iter1, typename Sent2 = Iter2>
void
test_range_impl()
{
	int ia[] = {0, 1, 2, 3, 4, 5};
	const unsigned sa = sizeof(ia)/sizeof(ia[0]);
	auto const all_of_a = subrange{Iter1(ia), Iter1(ia+sa)};
	CHECK(eq(subrange{Iter1(ia), Iter1(ia)},
		ranges::search(all_of_a, subrange(Iter2(ia), Sent2(ia)))));
	CHECK(eq(subrange{Iter1(ia), Iter1(ia+1)},
		ranges::search(all_of_a, subrange(Iter2(ia), Sent2(ia+1)))));
	CHECK(eq(subrange{Iter1(ia+1), Iter1(ia+2)},
		ranges::search(all_of_a, subrange(Iter2(ia+1), Sent2(ia+2)))));
	CHECK(eq(subrange{Iter1(ia), Iter1(ia)},
		ranges::search(all_of_a, subrange(Iter2(ia+2), Sent2(ia+2)))));
	CHECK(eq(subrange{Iter1(ia+2), Iter1(ia+3)},
		ranges::search(all_of_a, subrange(Iter2(ia+2), Sent2(ia+3)))));
	CHECK(eq(subrange{Iter1(ia), Iter1(ia)},
		ranges::search(subrange(Iter1(ia), Sent1(ia)), subrange(Iter2(ia+2), Sent2(ia+3)))));
	CHECK(eq(subrange{Iter1(ia+sa-3), Iter1(ia+sa)},
		ranges::search(all_of_a, subrange(Iter2(ia+sa-3), Sent2(ia+sa)))));
	CHECK(eq(subrange{Iter1(ia+sa-1), Iter1(ia+sa)},
		ranges::search(all_of_a, subrange(Iter2(ia+sa-1), Sent2(ia+sa)))));
	CHECK(eq(all_of_a,
		ranges::search(all_of_a, subrange(Iter2(ia), Sent2(ia+sa)))));
	CHECK(eq(subrange{Iter1(ia+sa-1), Iter1(ia+sa-1)},
		ranges::search(subrange(Iter1(ia), Sent1(ia+sa-1)), subrange(Iter2(ia), Sent2(ia+sa)))));
	CHECK(eq(subrange{Iter1(ia+1), Iter1(ia+1)},
		ranges::search(subrange(Iter1(ia), Sent1(ia+1)), subrange(Iter2(ia), Sent2(ia+sa)))));

	int ib[] = {0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 4};
	const unsigned sb = sizeof(ib)/sizeof(ib[0]);
	auto const all_of_b = subrange{Iter1(ib), Iter1(ib+sb)};
	int ic[] = {1};
	CHECK(eq(subrange{Iter1(ib+1), Iter1(ib+2)},
		ranges::search(all_of_b, subrange(Iter2(ic), Sent2(ic+1)))));

	int id[] = {1, 2};
	CHECK(eq(subrange{Iter1(ib+1), Iter1(ib+3)},
		ranges::search(all_of_b, subrange(Iter2(id), Sent2(id+2)))));

	int ie[] = {1, 2, 3};
	CHECK(eq(subrange{Iter1(ib+4), Iter1(ib+7)},
		ranges::search(all_of_b, subrange(Iter2(ie), Sent2(ie+3)))));

	int ig[] = {1, 2, 3, 4};
	CHECK(eq(subrange{Iter1(ib+8), Iter1(ib+12)},
		ranges::search(all_of_b, subrange(Iter2(ig), Sent2(ig+4)))));

	int ih[] = {0, 1, 1, 1, 1, 2, 3, 0, 1, 2, 3, 4};
	const unsigned sh = sizeof(ih)/sizeof(ih[0]);
	int ii[] = {1, 1, 2};
	CHECK(eq(subrange{Iter1(ih+3), Iter1(ih+6)},
		ranges::search(subrange(Iter1(ih), Sent1(ih+sh)),
			subrange(Iter2(ii), Sent2(ii+3)))));

	int ij[] = {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
	const unsigned sj = sizeof(ij)/sizeof(ij[0]);
	int ik[] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
	const unsigned sk = sizeof(ik)/sizeof(ik[0]);
	CHECK(eq(subrange{Iter1(ij+6), Iter1(ij+6+sk)},
		ranges::search(subrange(Iter1(ij), Sent1(ij+sj)),
			subrange(Iter2(ik), Sent2(ik+sk)))));
}

template<class Iter1, class Iter2>
void
test_range()
{
	using Sent1 = typename sentinel_type<Iter1>::type;
	using Sent2 = typename sentinel_type<Iter2>::type;
	test_range_impl<Iter1, Iter2>();
	test_range_impl<Iter1, Iter2, Sent1>();
	test_range_impl<Iter1, Iter2, Iter1, Sent2>();
	test_range_impl<Iter1, Iter2, Sent1, Sent2>();

	using SizedSent1 = typename sentinel_type<Iter1, true>::type;
	using SizedSent2 = typename sentinel_type<Iter2, true>::type;
	test_range_impl<Iter1, Iter2, SizedSent1, SizedSent2>();
}

template<class Iter1, class Iter2>
void
test()
{
	test_iter<Iter1, Iter2>();
	test_range<Iter1, Iter2>();
}

struct S
{
	int i;
};

struct T
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
		S const in[] = {{0}, {1}, {2}, {3}, {4}, {5}};
		T const pat[] = {{2}, {3}};

		auto result = ranges::search(in, pat, ranges::equal_to{}, &S::i, &T::i);
		CHECK(result.begin() == in+2);
		CHECK(result.end() == in+4);
	}

	// Test counted ranges
	{
		int in[] = {0,1,2,3,4,5};
		auto rng = subrange(
					 ranges::make_counted_iterator(
					   bidirectional_iterator<int*>(in), 6),
					 ranges::default_sentinel{});
		{
			auto [b, e] = ranges::search(rng, std::initializer_list<int>{2,3});
			CHECK(base(b.base()) == in+2);
			CHECK(b.count() == 4);
			CHECK((e - b) == 2);
		}

		auto [b, e] = ranges::search(rng, std::initializer_list<int>{5,6});
		CHECK(base(b.base()) == in+6);
		CHECK(b.count() == 0);
		CHECK((e - b) == 0);
	}

	return ::test_result();
}
