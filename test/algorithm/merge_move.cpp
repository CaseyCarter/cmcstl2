// Range v3 library
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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

#include <stl2/detail/algorithm/merge_move.hpp>
#include <algorithm>
#include <memory>
#include <utility>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

struct S {
  std::unique_ptr<int> iptr;

  S() = default;
  S(int ii) : iptr{std::make_unique<int>(ii)} {}

  explicit operator bool() const {
    return iptr != nullptr;
  }
};

int main()
{
    {
        unsigned N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(unsigned i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(unsigned i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r = stl2::merge_move(ia.get(), ia.get() + N,
            ib.get(), ib.get() + N, ic.get());
        CHECK(std::get<0>(r) == ia.get() + N);
        CHECK(std::get<1>(r) == ib.get() + N);
        CHECK(std::get<2>(r) == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }

    {
        unsigned N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(unsigned i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(unsigned i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r0 = stl2::ext::make_range(ia.get(), ia.get() + N);
        auto r1 = stl2::ext::make_range(ib.get(), ib.get() + N);
        auto r = stl2::merge_move(r0, r1, ic.get());
        CHECK(std::get<0>(r) == ia.get() + N);
        CHECK(std::get<1>(r) == ib.get() + N);
        CHECK(std::get<2>(r) == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }


    {
        unsigned N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(unsigned i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(unsigned i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r0 = stl2::ext::make_range(ia.get(), ia.get() + N);
        auto r1 = stl2::ext::make_range(ib.get(), ib.get() + N);
        auto r = stl2::merge_move(std::move(r0), std::move(r1), ic.get());
        CHECK(std::get<0>(r).get_unsafe() == ia.get() + N);
        CHECK(std::get<1>(r).get_unsafe() == ib.get() + N);
        CHECK(std::get<2>(r) == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }

    {
        auto comp = [](const S& lhs, const S& rhs) {
          STL2_ASSERT(lhs.iptr);
          STL2_ASSERT(rhs.iptr);
          return *lhs.iptr < *rhs.iptr;
        };
        constexpr int N = 1U << 8;
        S ia[N];
        S ib[N];
        S ic[2 * N];
        for(auto i = 0; i < N; ++i)
            ia[i] = S{2 * i};
        for(auto i = 0; i < N; ++i)
            ib[i] = S{2 * i + 1};
        auto r = stl2::merge_move(ia + 0, ia + N, ib + 0, ib + N, ic + 0, comp);
        CHECK(std::get<0>(r) == ia + N);
        CHECK(std::get<1>(r) == ib + N);
        CHECK(std::get<2>(r) == ic + 2 * N);
        CHECK(std::none_of(ia, ia + N, stl2::identity{}));
        CHECK(std::none_of(ib, ib + N, stl2::identity{}));
        CHECK(std::all_of(ic, ic + 2 * N, stl2::identity{}));
        CHECK(*ic[0].iptr == 0);
        CHECK(*ic[2 * N - 1].iptr == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic, ic + 2 * N, comp));
    }

    return ::test_result();
}
