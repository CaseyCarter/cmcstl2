#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges_v1/algorithm>
#include <experimental/ranges_v1/iterator>

// Qualify everything with "std::experimental::ranges_v1" if you like,
// I prefer to use a namespace alias:
namespace rng = std::experimental::ranges_v1;

int main() {
  auto v = std::vector<std::string>{"this", "is", "a", "test"};
  auto out = rng::ostream_iterator<std::string>{std::cout, " "};
  rng::sort(v);
  rng::copy(v, out);
  std::cout << '\n';
  auto result = rng::reverse_copy(v, out);
  assert(result.in() == rng::end(v));
  std::cout << '\n';
}
