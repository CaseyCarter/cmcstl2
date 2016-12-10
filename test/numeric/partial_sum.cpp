#include <stl2/detail/numeric/partial_sum.hpp>
#include "../simple_test.hpp"
#include <numeric>
#include <vector>

namespace ranges = __stl2;

int main()
{
   {
      const std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      std::vector<int> standard(v.size());
      std::partial_sum(v.begin(), v.end(), standard.begin());

      std::vector<int> range_eq(v.size());
      auto range_pair = ranges::partial_sum(v.begin(), v.end(), range_eq.begin());

      CHECK(range_eq == standard);
      CHECK(range_pair.in() == v.end());
      CHECK(range_pair.out() == range_eq.end());
   }
   {
      const std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      std::vector<int> standard(v.size());
      std::partial_sum(v.begin(), v.end(), standard.begin());

      std::vector<int> range_eq(v.size());
      auto range_pair = ranges::partial_sum(v.begin(), v.end(), range_eq.begin());

      CHECK(range_eq == standard);
      CHECK(range_pair.in() == v.end());
      CHECK(range_pair.out() == range_eq.end());
   }

   return test_result();
}