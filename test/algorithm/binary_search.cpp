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

#include <stl2/detail/algorithm/binary_search.hpp>
#include <stl2/view/iota.hpp>
#include <utility>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

int main()
{
    using stl2::begin;
    using stl2::end;
    using stl2::size;
    using stl2::less;

    std::pair<int, int> a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    const std::pair<int, int> c[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};

    CHECK(stl2::binary_search(begin(a), end(a), a[0]));
    CHECK(stl2::binary_search(begin(a), end(a), a[1], less<>()));
    CHECK(stl2::binary_search(begin(a), end(a), 1, less<>(), &std::pair<int, int>::first));

    CHECK(stl2::binary_search(a, a[2]));
    CHECK(stl2::binary_search(c, c[3]));

    CHECK(stl2::binary_search(a, a[4], less<>()));
    CHECK(stl2::binary_search(c, c[5], less<>()));

    CHECK(stl2::binary_search(a, 1, less<>(), &std::pair<int, int>::first));
    CHECK(stl2::binary_search(c, 1, less<>(), &std::pair<int, int>::first));

    CHECK(stl2::binary_search(a, 0, less<>(), &std::pair<int, int>::first));
    CHECK(stl2::binary_search(c, 0, less<>(), &std::pair<int, int>::first));

    CHECK(!stl2::binary_search(a, -1, less<>(), &std::pair<int, int>::first));
    CHECK(!stl2::binary_search(c, -1, less<>(), &std::pair<int, int>::first));

    CHECK(!stl2::binary_search(a, 4, less<>(), &std::pair<int, int>::first));
    CHECK(!stl2::binary_search(c, 4, less<>(), &std::pair<int, int>::first));

    CHECK(stl2::binary_search(stl2::iota_view<int>{0}, 42));

    return test_result();
}
