#include <stl2/detail/iterator/istreambuf_iterator.hpp>
#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <sstream>
#include "../simple_test.hpp"

using namespace __stl2;

int main() {
  using I = istreambuf_iterator<char>;
  static_assert(models::InputIterator<I>);
  static_assert(models::Sentinel<default_sentinel, I>);
  using C = CommonType<I, default_sentinel>;
  static_assert(is_same<C, I>());

  {
    static const char hw[] = "Hello, world!";
    std::istringstream is(hw);
    ::check_equal(ext::make_range(I{is}, default_sentinel{}),
                  ext::make_range(hw + 0, hw + size(hw) - 1));
  }

  {
    // Test the postincrement proxy type.
    std::istringstream is("123");
    auto i = I{is};
    CHECK(*i++ == '1');
    auto j = i++;
    CHECK(*j == '2');
    auto k = I{j};
    CHECK(*k++ == '3');
    CHECK(k == I{});
  }

  {
    // Test the operator-> proxy type.
    std::istringstream is("123");
    auto i = I{is};
    CHECK(*i.operator->().operator->() == '1');
    ++i;
    CHECK(*i.operator->().operator->() == '2');
  }

  return ::test_result();
}
