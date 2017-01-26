// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/algorithm.hpp>
#include <list>
#include "../test_iterators.hpp"
#include "../simple_test.hpp"

int main()
{
	using namespace __stl2;

	{
		int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		auto i = make_counted_iterator(forward_iterator<int*>{rgi}, size(rgi));
		static_assert(is_same<decltype(i),counted_iterator<forward_iterator<int*>>>());
		static_assert(models::SizedSentinel<default_sentinel, decltype(i)>);
		CHECK(static_cast<std::size_t>(default_sentinel{} - i) == size(rgi));
		CHECK(&*i.base() == begin(rgi));
		CHECK(std::size_t(i.count()) == size(rgi));
		CHECK(std::size_t(distance(i, default_sentinel{})) == size(rgi));

		counted_iterator<forward_iterator<const int*>> j{i};
		using C = common_iterator<decltype(i), default_sentinel>;
		CHECK(std::equal(C{i}, C{default_sentinel{}}, rgi));
	}

	{
		std::list<int> l;
		auto a = make_counted_iterator(l.begin(), 0);
		auto b = make_counted_iterator(l.cbegin(), 0);
		static_assert(is_same<common_type_t<decltype(a), decltype(b)>, decltype(b)>());
		CHECK((a - a) == 0);
		CHECK((b - b) == 0);
		CHECK((a - b) == 0);
		CHECK((b - a) == 0);
	}

	{
		counted_iterator<char*> c{nullptr, 0};
		counted_iterator<char const*> d{c};
		static_assert(!models::Assignable<decltype(c)&, decltype(d)>);
		CHECK((c - c) == 0);
		CHECK((d - d) == 0);
		CHECK((c - d) == 0);
		CHECK((d - c) == 0);
	}

	{
		int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		counted_iterator<output_iterator<int*>> e{output_iterator<int*>{rgi}, 10};
		fill(e, default_sentinel{}, 0);
		int expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		CHECK(std::equal(rgi, rgi + size(rgi), expected));
		// Make sure advance compiles
		advance(e, 4);
		CHECK(e.base().base() == rgi + 4);
		CHECK(e.count() == 10 - 4);
	}

	return ::test_result();
}
