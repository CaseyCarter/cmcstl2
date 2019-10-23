// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Dvir Yitzchaki 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include <stl2/view/const.hpp>
#include <stl2/view/counted.hpp>
#include <stl2/view/filter.hpp>
#include <stl2/view/move.hpp>
#include <vector>

namespace ranges = __stl2;

int main() {
  using namespace ranges;

  int rgi[] = {1, 2, 3, 4};

  {
    auto rng = rgi | views::ext::as_const;
    static_assert(same_as<int &, decltype(*begin(rgi))>);
    static_assert(same_as<int const &, decltype(*begin(rng))>);
    static_assert(
        same_as<int const &&, range_rvalue_reference_t<decltype(rng)>>);
    static_assert(view<decltype(rng)>);
    static_assert(common_range<decltype(rng)>);
    static_assert(sized_range<decltype(rng)>);
    static_assert(random_access_range<decltype(rng)>);
    CHECK_EQUAL(rng, {1, 2, 3, 4});
    CHECK(&*begin(rng) == &rgi[0]);
    CHECK(rng.size() == 4u);
  }

  {
    auto rng2 =
        views::counted(::forward_iterator(rgi), 4) | views::ext::as_const;
    static_assert(same_as<int const &, decltype(*begin(rng2))>);
    static_assert(
        same_as<range_rvalue_reference_t<decltype(rng2)>, int const &&>);
    static_assert(view<decltype(rng2)>);
    static_assert(forward_range<decltype(rng2)>);
    static_assert(!bidirectional_range<decltype(rng2)>);
    static_assert(!common_range<decltype(rng2)>);
    static_assert(sized_range<decltype(rng2)>);
    CHECK_EQUAL(rng2, {1, 2, 3, 4});
    CHECK(&*begin(rng2) == &rgi[0]);
    CHECK(rng2.size() == 4u);
  }

#if 0 // Test DISABLED pending view implementations.
  {
	  auto zip = views::zip(rgi, rgi);
	  auto rng3 = zip | views::ext::as_const;
	  has_type<common_pair<int &, int &>>(*begin(zip));
	  has_type<common_pair<int &&, int &&>>(iter_move(begin(zip)));
	  has_type<common_pair<int const &, int const &>>(*begin(rng3));
	  has_type<common_pair<int const &&, int const &&>>(iter_move(begin(rng3)));
	  static_assert(view<decltype(rng3)>);
	  static_assert(random_access_range<decltype(rng3)>);
	  static_assert(common_range<decltype(rng3)>);
	  static_assert(sized_range<decltype(rng3)>);
	  using P = std::pair<int,int>;
	  CHECK_EQUAL(rng3, {P{1,1}, P{2,2}, P{3,3}, P{4,4}});
	  CHECK(&(*begin(rng3)).first == &rgi[0]);
	  CHECK(rng3.size() == 4u);
  }

  {
	  auto zip2 = views::zip(rgi, rgi) | views::move;
	  auto rng4 = zip2 | views::ext::as_const;
	  has_type<common_pair<int &&, int &&>>(*begin(zip2));
	  has_type<common_pair<int &&, int &&>>(iter_move(begin(zip2)));
	  has_type<common_pair<int const &&, int const &&>>(*begin(rng4));
	  has_type<common_pair<int const &&, int const &&>>(iter_move(begin(rng4)));
	  static_assert(view<decltype(rng4)>);
	  static_assert(random_access_range<decltype(rng4)>);
	  static_assert(common_range<decltype(rng4)>);
	  static_assert(sized_range<decltype(rng4)>);
	  using P = std::pair<int,int>;
	  CHECK_EQUAL(rng4, {P{1,1}, P{2,2}, P{3,3}, P{4,4}});
	  CHECK(&(*begin(rng4)).first == &rgi[0]);
	  CHECK(rng4.size() == 4u);
  }

  {
	  auto rng = debug_input_view<int>{rgi} | views::ext::as_const;
	  static_assert(same_as<int const&, range_reference_t<decltype(rng)>>);
	  CHECK_EQUAL(rng, rgi);
  }
#endif

  {
    auto rng6 =
        rgi | views::filter([](auto) { return true; }) | views::ext::as_const;
    static_assert(view<decltype(rng6)>);
  }

  {
    auto rng7 = rgi | views::move | views::ext::as_const;
    static_assert(same_as<int const &&, decltype(*begin(rng7))>);
    static_assert(
        same_as<int const &&, range_rvalue_reference_t<decltype(rng7)>>);
    static_assert(view<decltype(rng7)>);
    static_assert(common_range<decltype(rng7)>);
    static_assert(sized_range<decltype(rng7)>);
    static_assert(!forward_range<decltype(rng7)>);
    CHECK_EQUAL(rng7, {1, 2, 3, 4});
    CHECK(rng7.size() == 4u);
    // no move of const rvalue
    CHECK_EQUAL(rgi, {1, 2, 3, 4});
  }

  return ::test_result();
}
