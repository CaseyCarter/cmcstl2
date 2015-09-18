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

#include <stl2/detail/algorithm/copy_backward.hpp>
#include <cstring>
#include <utility>
#include <algorithm>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

int main()
{
    using stl2::begin;
    using stl2::end;
    using stl2::size;

    std::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    static_assert(size(a) == 6, "");
    std::pair<int, int> out[size(a)] = {};

    auto res = stl2::copy_backward(begin(a), end(a), end(out));
    CHECK(res.first == end(a));
    CHECK(res.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    CHECK(!std::equal(a, a + size(a), out));

    res = stl2::copy_backward(a, end(out));
    CHECK(res.first == end(a));
    CHECK(res.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    auto res2 = stl2::copy_backward(stl2::move(a), end(out));
    CHECK(res2.first.get_unsafe() == end(a));
    CHECK(res2.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    return test_result();
}
