//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

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
  }

  return test_result();
}
