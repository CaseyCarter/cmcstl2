#include <stl2/detail/iterator/istream_iterator.hpp>
#include <stl2/type_traits.hpp>
#include <sstream>
#include "../simple_test.hpp"

int main() {
  {
    using I = stl2::istream_iterator<int>;
    using C = stl2::CommonType<I, stl2::default_sentinel>;
    static_assert(stl2::is_same<C, I>());
  }
  {
    std::istringstream is("5 4 3 2 1 0");
    stl2::istream_iterator<int> i{is};
    int j = 5;
    for (; i != stl2::default_sentinel{}; ++i, --j) {
      CHECK(*i == j);
    }
    CHECK(-1 == j);
  }
  {
    std::istringstream is("0.9 1.8 2.4 3.3");
    static constexpr double values[] = {0.9, 1.8, 2.4, 3.3};
    CHECK(std::equal(
      stl2::istream_iterator<double>{is}, stl2::istream_iterator<double>{},
      stl2::begin(values), stl2::end(values)));
  }
  return test_result();
}
