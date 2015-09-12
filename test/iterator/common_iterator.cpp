// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <algorithm>
#include <stl2/iterator.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

int main() {
  {
    namespace models = ::__stl2::models;
    static_assert(
        models::ForwardIterator<
          __stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>>>,
        "");
    static_assert(
        !models::BidirectionalIterator<
          __stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>>>,
        "");
    static_assert(
      std::is_same<
        __stl2::common_type<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >::type,
        __stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        __stl2::common_type<
          sentinel<const char *>,
          bidirectional_iterator<const char *>
        >::type,
        __stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        __stl2::common_reference<
          const bidirectional_iterator<const char *>&,
          const sentinel<const char *>&
        >::type,
        __stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        __stl2::common_reference<
          const sentinel<const char *>&,
          const bidirectional_iterator<const char *>&
        >::type,
        __stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
      std::is_same<
        __stl2::common_reference<
          __stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>
          >&,
          __stl2::common_iterator<
            bidirectional_iterator<const char *>,
            sentinel<const char *>
          >
        >::type,
        __stl2::common_iterator<
          bidirectional_iterator<const char *>,
          sentinel<const char *>
        >
      >::value, ""
    );
    static_assert(
        models::Common<
          bidirectional_iterator<const char *>,
          sentinel<const char *>>,
        "");
    // Sized iterator range tests
    static_assert(
      !models::SizedIteratorRange<
        __stl2::common_iterator<
          forward_iterator<int*>,
          sentinel<int*, true> >,
        __stl2::common_iterator<
          forward_iterator<int*>,
          sentinel<int*, true> > >,
        "");
    static_assert(
      models::SizedIteratorRange<
        __stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, true> >,
        __stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, true> > >,
        "");
    static_assert(
      !models::SizedIteratorRange<
        __stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, false> >,
        __stl2::common_iterator<
          random_access_iterator<int*>,
          sentinel<int*, false> > >,
        "");
  }
  {
    int rgi[] {0,1,2,3,4,5,6,7,8,9};
    using CI = __stl2::common_iterator<
      random_access_iterator<int*>,
      sentinel<int*>>;
    CI first{random_access_iterator<int*>{rgi}};
    CI last{sentinel<int*>{rgi+10}};
    CHECK(std::accumulate(first, last, 0, std::plus<int>{}) == 45);
  }
  return test_result();
}
