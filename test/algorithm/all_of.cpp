#include <stl2/detail/algorithm/all_of.hpp>

#include "../simple_test.hpp"

int main() {
  static const int even_ints[] = {8, 12, 0, 14};
  CHECK(stl2::all_of(even_ints, [](auto&& i){ return i % 2 == 0; }));
  return ::test_result();
}
