#include <stl2/detail/numeric/accumulate.hpp>
#include "../detail/int128.hpp"
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
      constexpr auto init = 0.0;
      const auto a = std::accumulate(d.begin(), d.end(), init);
      CHECK(ranges::accumulate(d.begin(), d.end(), init) == a);
      CHECK(ranges::accumulate(d, init) == a);
   }
   {
      using namespace std::string_literals;
      auto l = std::list<std::string>{"hello"s, "how are you"s, "foo"s, "bar"s,
                                       "this"s, "that"s, "tit"s, "for"s, "tat"s};
      const auto a = std::accumulate(l.cbegin(), l.cend(), ""s);
      CHECK(ranges::accumulate(l.cbegin(), l.cend()) == a);
      CHECK(ranges::accumulate(l) == a);
   }
   {
      const auto v = std::vector<cmcstl2::Uint128>{};
      const auto a = std::accumulate(v.begin(), v.end(), cmcstl2::Uint128{});
      CHECK(ranges::accumulate(v) == a);
   }
   {
      const auto v = std::vector<cmcstl2::Uint128>(100, cmcstl2::Uint128{0xdeadbeef, 0xf00d4b0b});
      const auto a = std::accumulate(v.begin(), v.end(), cmcstl2::Uint128{});
      CHECK(ranges::accumulate(v) == a);
   }
   {
      const auto v = std::vector<double>{};
      const auto a = std::accumulate(v.begin(), v.end(), 0);
      CHECK(ranges::accumulate(v) == a);
   }
   {
      const auto v = std::vector<double>{0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};
      constexpr auto init = 0;
      const auto a = std::accumulate(v.begin(), v.end(), init);
      CHECK(ranges::accumulate(v, init) == a);
   }

   return test_result();
}