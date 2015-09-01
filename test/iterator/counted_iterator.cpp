// Stolen from Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <list>
#include "test_iterators.hpp"
#include "../simple_test.hpp"

int main()
{
    using namespace stl2;
    namespace models = stl2::ext::models;

    {
        int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto i = make_counted_iterator(forward_iterator<int*>{rgi}, size(rgi));
        static_assert(is_same<decltype(i),counted_iterator<forward_iterator<int*>>>());
        static_assert(models::sized_iterator_range<decltype(i), default_sentinel>());
        CHECK(static_cast<std::size_t>(default_sentinel{} - i) == size(rgi));
        CHECK(&*i.base() == begin(rgi));
        CHECK(std::size_t(i.count()) == size(rgi));
        CHECK(std::size_t(distance(i, default_sentinel{})) == size(rgi));

        counted_iterator<forward_iterator<const int*>> j{i};
        using C = CommonType<decltype(i), default_sentinel>;
        static_assert(is_same<C, common_iterator<decltype(i), default_sentinel>>());
        CHECK(std::equal(C{i}, C{default_sentinel{}}, rgi));
    }

    {
        std::list<int> l;
        auto a = make_counted_iterator(l.begin(), 0);
        auto b = make_counted_iterator(l.cbegin(), 0);
        static_assert(is_same<CommonType<decltype(a), decltype(b)>, decltype(b)>());
        CHECK((a - a) == 0);
        CHECK((b - b) == 0);
        CHECK((a - b) == 0);
        CHECK((b - a) == 0);
    }

    {
        counted_iterator<char*> c{nullptr, 0};
        counted_iterator<char const*> d{c};
        static_assert(!models::assignable<decltype(c)&, decltype(d)>());
        CHECK((c - c) == 0);
        CHECK((d - d) == 0);
        CHECK((c - d) == 0);
        CHECK((d - c) == 0);
    }

    return ::test_result();
}
