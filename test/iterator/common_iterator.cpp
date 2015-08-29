//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <algorithm>
#include <stl2/iterator.hpp>
#include "../simple_test.hpp"
#include "test_iterators.hpp"

int main() {
  {
    namespace models = stl2::ext::models;
    static_assert(
        models::forward_iterator<
          stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>>>(),
        "");
    static_assert(
        !models::bidirectional_iterator<
          stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>>>(),
        "");
    static_assert(
      std::is_same<
        stl2::common_type<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >::type,
        stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        stl2::common_type<
          sentinel<const char *>,
          bidirectional_iterator<const char *>
        >::type,
        stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        stl2::common_reference<
          const bidirectional_iterator<const char *>&,
          const sentinel<const char *>&
        >::type,
        stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        stl2::common_reference<
          const sentinel<const char *>&,
          const bidirectional_iterator<const char *>&
        >::type,
        stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        stl2::common_reference<
          stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>
          >&,
          stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>
          >
        >::type,
        stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
        models::common<
          bidirectional_iterator<const char *>,
          sentinel<const char *>>(),
        "");
    // Sized iterator range tests
    static_assert(
      !models::sized_iterator_range<
        stl2::common_iterator<
          forward_iterator<int*>,
          sentinel<int*, true> >,
        stl2::common_iterator<
          forward_iterator<int*>,
          sentinel<int*, true> > >(),
        "");
    static_assert(
      models::sized_iterator_range<
        stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, true> >,
        stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, true> > >(),
        "");
    static_assert(
      !models::sized_iterator_range<
        stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, false> >,
        stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, false> > >(),
        "");
  }
  {
    int rgi[] {0,1,2,3,4,5,6,7,8,9};
    using CI = stl2::common_iterator<
      random_access_iterator<int*>,
      sentinel<int*>>;
    CI first{random_access_iterator<int*>{rgi}};
    CI last{sentinel<int*>{rgi+10}};
    CHECK(std::accumulate(first, last, 0, std::plus<int>{}) == 45);
  }
  return test_result();
}
