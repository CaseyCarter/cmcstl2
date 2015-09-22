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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stl2/detail/algorithm/partition_move.hpp>
#include <stl2/algorithm.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <tuple>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace stl2 = __stl2;

namespace {
  struct SS {
    int i;

    SS() = default;
    SS(int ii) : i{ii} {}
    SS(SS&& that) :
      i{stl2::exchange(that.i, 0)} {}
    SS(const SS&) = default;

    SS& operator=(SS&& that) & {
      i = stl2::exchange(that.i, 0);
      return *this;
    }
    SS& operator=(const SS&) & = default;

    SS& operator=(int ii) {
      i = ii;
      return *this;
    }

    friend bool operator==(const SS& lhs, const SS& rhs) {
      return lhs.i == rhs.i;
    }
    friend bool operator!=(const SS& lhs, const SS& rhs) {
      return !(lhs == rhs);
    }
  };

  constexpr struct {
    constexpr bool operator()(const int& i) const {
      return i % 2 != 0;
    }
  } is_odd{};

  template <class Iter, class Sent = Iter>
  void
  test_iter()
  {
    SS ia[8] = {1, 2, 3, 4, 6, 8, 5, 7};
    SS r1[8] = {};
    SS r2[8] = {};
    auto p = stl2::partition_move(Iter(std::begin(ia)),
                                  Sent(std::end(ia)),
                                  output_iterator<SS*>(r1), r2, is_odd, &SS::i);
    CHECK(std::get<0>(p) == Iter(std::end(ia)));
    CHECK(std::get<1>(p).base() == r1 + 4);
    CHECK(r1[0] == 1);
    CHECK(r1[1] == 3);
    CHECK(r1[2] == 5);
    CHECK(r1[3] == 7);
    CHECK(stl2::count(r1 + 4, std::end(r1), 0, &SS::i) == 4);
    CHECK(std::get<2>(p) == r2 + 4);
    CHECK(r2[0] == 2);
    CHECK(r2[1] == 4);
    CHECK(r2[2] == 6);
    CHECK(r2[3] == 8);
    CHECK(stl2::count(r2 + 4, std::end(r2), 0, &SS::i) == 4);
    CHECK(stl2::count(ia, 0, &SS::i) == 8);
  }

  template <class Iter, class Sent = Iter>
  void
  test_range()
  {
    SS ia[8] = {1, 2, 3, 4, 6, 8, 5, 7};
    SS r1[8] = {};
    SS r2[8] = {};
    auto rng = stl2::ext::make_range(Iter(std::begin(ia)), Sent(std::end(ia)));
    auto p = stl2::partition_move(rng, output_iterator<SS*>(r1), r2, is_odd, &SS::i);
    CHECK(std::get<0>(p) == Iter(std::end(ia)));
    CHECK(std::get<1>(p).base() == r1 + 4);
    CHECK(r1[0].i == 1);
    CHECK(r1[1].i == 3);
    CHECK(r1[2].i == 5);
    CHECK(r1[3].i == 7);
    CHECK(stl2::count(r1 + 4, std::end(r1), 0, &SS::i) == 4);
    CHECK(std::get<2>(p) == r2 + 4);
    CHECK(r2[0].i == 2);
    CHECK(r2[1].i == 4);
    CHECK(r2[2].i == 6);
    CHECK(r2[3].i == 8);
    CHECK(stl2::count(r2 + 4, std::end(r2), 0, &SS::i) == 4);
    CHECK(stl2::count(ia, 0, &SS::i) == 8);
  }

  void test_rvalue()
  {
    // Test rvalue ranges
    SS ia[8] = {1, 2, 3, 4, 6, 8, 5, 7};
    SS r1[8] = {};
    SS r2[8] = {};
    auto p = stl2::partition_move(stl2::move(ia), r1, r2, is_odd, &SS::i);
    CHECK(std::get<0>(p).get_unsafe() == std::end(ia));
    CHECK(std::get<1>(p) == r1 + 4);
    CHECK(r1[0].i == 1);
    CHECK(r1[1].i == 3);
    CHECK(r1[2].i == 5);
    CHECK(r1[3].i == 7);
    CHECK(stl2::count(r1 + 4, std::end(r1), 0, &SS::i) == 4);
    CHECK(std::get<2>(p) == r2 + 4);
    CHECK(r2[0].i == 2);
    CHECK(r2[1].i == 4);
    CHECK(r2[2].i == 6);
    CHECK(r2[3].i == 8);
    CHECK(stl2::count(r2 + 4, std::end(r2), 0, &SS::i) == 4);
    CHECK(stl2::count(ia, 0, &SS::i) == 8);
  }
} // unnamed namespace

int main()
{
    test_iter<input_iterator<SS*>>();
    test_iter<input_iterator<SS*>, sentinel<SS*>>();

    test_range<input_iterator<SS*>>();
    test_range<input_iterator<SS*>, sentinel<SS*>>();

    test_rvalue();

    return ::test_result();
}
