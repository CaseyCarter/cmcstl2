#include <stl2/detail/numeric/adjacent_difference.hpp>
#include "../simple_test.hpp"
#include <iterator>
#include <numeric>
#include <stl2/iterator.hpp>
#include <vector>

#include <iostream>
#include <stl2/algorithm.hpp>

namespace ranges = __stl2;

int main()
{
   {
      const std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

      std::vector<int> s(v.size());
      std::adjacent_difference(v.begin(), v.end(), s.begin());

      std::vector<int> r(v.size());
      auto result = ranges::adjacent_difference(v.begin(), v.end(), r.begin());

      CHECK(s == r);
      CHECK(result.in() == v.end());
      CHECK(result.out() == r.end());
   }

   return test_result();
}