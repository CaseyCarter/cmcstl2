#include <stl2/detail/numeric/accumulate.hpp>
#include "../simple_test.hpp"
#include <deque>
#include <list>
#include <numeric>
#include <string>
#include <vector>

namespace ranges = __stl2;

int main()
{
   {
      auto v = std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      auto a = std::accumulate(v.begin(), v.end(), 0);
      constexpr auto init = 0;

      CHECK(ranges::accumulate(v.begin(), v.end(), init) == a);
      CHECK(ranges::accumulate(v, init) == a);

      auto foo = [](const auto a, const auto b) { return a < b ? a - b : a + b; };
      a = std::accumulate(v.begin(), v.end(), init, foo);
      CHECK(ranges::accumulate(v.begin(), v.end(), init, foo) == a);
      CHECK(ranges::accumulate(v, init, foo) == a);
   }

   {
      const auto d = std::deque<double>{0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
      auto a = std::accumulate(d.begin(), d.end(), 0);
      CHECK(ranges::accumulate(d.begin(), d.end(), 0) == a);
      CHECK(ranges::accumulate(d, 0) == a);
   }

   {
      using namespace std::string_literals;
      auto l = std::list<std::string>{"hello"s, "how are you"s, "foo"s, "bar"s,
                                       "this"s, "that"s, "tit"s, "for"s, "tat"s};
      auto a = std::accumulate(l.cbegin(), l.cend(), ""s);
      CHECK(ranges::accumulate(l.cbegin(), l.cend()) == a);
      CHECK(ranges::accumulate(l) == a);
   }

   return test_result();
}