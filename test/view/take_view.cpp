// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/take.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/filter.hpp>
#include <stl2/detail/algorithm/count.hpp>
#include <stl2/detail/algorithm/transform.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <memory>
#include <vector>
#include "../simple_test.hpp"

namespace ranges = __stl2;

int main()
{
    using namespace ranges;

    {
        auto rng = view::iota(0) | view::take(10);
        static_assert(models::View<decltype(rng)>);
        static_assert(!models::SizedRange<ext::iota_view<int, false>>);
        static_assert(models::Range<const decltype(rng)>);
        ::check_equal(rng, {0,1,2,3,4,5,6,7,8,9});
        decltype(rng)::const_iterator i{};
    }

    {
        auto rng = view::iota(0, 100) | view::take(10);
        static_assert(models::View<decltype(rng)>);
        static_assert(models::Range<const decltype(rng)>);
        ::check_equal(rng, {0,1,2,3,4,5,6,7,8,9});
        decltype(rng)::const_iterator i{};
    }

    {
        auto evens = [](int i){return i%2 == 0;};
        std::vector<int> is = view::iota(0, 10);
        auto rng = is | view::filter(evens) | view::take(3);
        static_assert(models::View<decltype(rng)>);
        static_assert(!models::Range<const decltype(rng)>);
        decltype(rng)::iterator i{};
        ::check_equal(rng, {0,2,4});
    }

    return ::test_result();
}
