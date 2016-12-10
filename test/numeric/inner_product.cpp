#include <stl2/detail/numeric/inner_product.hpp>
#include "../simple_test.hpp"
#include <iterator>
#include <numeric>
#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <vector>
#include <iterator>
namespace ranges = __stl2;

int main()
{
   const std::vector<int> u{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   const std::vector<int> v{10, 20, 30, 40, 50, 60, 70, 80, 90};

   auto s = std::inner_product(u.begin(), u.end(), v.begin(), 0);

   CHECK(s == ranges::inner_product(u.begin(), u.end(), v.begin()));
   CHECK(s == ranges::inner_product(u, v.begin()));
   return test_result();
}