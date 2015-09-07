#include <stl2/algorithm.hpp>
#include <stl2/detail/algorithm/any_of.hpp>
#include "../simple_test.hpp"

namespace stl2 = ::__stl2;

template <class> class show_type;

int main() {
  {
    bool bools[] = {false, true, false};
    CHECK(stl2::any_of(bools, stl2::identity{}));
  }
  {
    static const int even_ints[] = {8, 12, 0, 14};
    CHECK(!stl2::any_of(even_ints, [](auto&& i){ return i % 2 == 1; }));
  }
  {
    struct A { bool b; };
    A bools[] = {{false}, {false}, {true}};
    CHECK(stl2::any_of(bools, stl2::identity{}, &A::b));
    CHECK(stl2::any_of(stl2::move(bools), stl2::identity{}, &A::b));
  }
  return test_result();
}
