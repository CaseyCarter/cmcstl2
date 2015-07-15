#include <stl2/iterator.hpp>

#include "simple_test.hpp"

int main() {
  {
    int a[] = { 0, 1 };
    stl2::iter_swap2(a + 0, a + 1);
    CHECK(a[0] == 1);
    CHECK(a[1] == 0);
  }

  return ::test_result();
}
