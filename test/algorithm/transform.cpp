#include <stl2/detail/algorithm/transform.hpp>

#include "../simple_test.hpp"

int main() {
  int rgi[]{1,2,3,4,5};
  __stl2::transform(rgi, rgi+5, rgi, [](int i){return i*2;});
  ::check_equal(rgi, {2,4,6,8,10});

  __stl2::transform(rgi, rgi, [](int i){return i/2;});
  ::check_equal(rgi, {1,2,3,4,5});

  return ::test_result();
}
