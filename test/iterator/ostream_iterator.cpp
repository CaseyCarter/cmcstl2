// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <sstream>
#include "../simple_test.hpp"

using namespace __stl2;

namespace {
  template <InputIterator I, Sentinel<I> S, OutputIterator<reference_t<I>> O>
  tagged_pair<tag::in(I), tag::out(O)>
  constexpr copy(I first, S last, O out) {
    for (; first != last; ++first, void(), ++out) {
      *out = *first;
    }
    return {first, out};
  }
}

int main() {
  using I = ostream_iterator<int>;
  static_assert(models::OutputIterator<I, const int&>);
  std::stringstream ss;
  
  static constexpr int some_ints[] = {0, 7, 1, 6, 2, 5, 3, 4};

  ::copy(__stl2::begin(some_ints), __stl2::end(some_ints), I{ss, " "});
  CHECK(ss.str() == "0 7 1 6 2 5 3 4 ");
  ::check_equal(
    ext::make_range(istream_iterator<int>{ss}, default_sentinel{}),
      some_ints);

  return ::test_result();
}
