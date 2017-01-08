#ifndef STL2_DETAIL_NUMERIC_GCD_HPP
#define STL2_DETAIL_NUMERIC_GCD_HPP

#include <cmath>
#include <stl2/tuple.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/algorithm/minmax.hpp>
#include <stl2/detail/concepts/number.hpp>

STL2_OPEN_NAMESPACE {
template <class M, class N, class T = common_type_t<M, N>>
   requires
      models::Number<M, N> &&
      models::Number<N, M> &&
      requires(T t) {
         {t % t}  -> T;
         {t %= t} -> T&;
      }
constexpr T gcd(M m, N n)
{
   constexpr auto zero = T{0};
   // TODO: replace with structured bindings when compiler support exists
   T a{};
   T b{};
   {
      auto p = __stl2::minmax(static_cast<T>(std::abs(m)), static_cast<T>(std::abs(n)));
      a = p.first;
      b = p.second;
   }
   // end TODO

   if (a == zero || b == zero)
      return T(0);

   // b = ac + r
   for (auto r = b % a; r != zero; r = b % a) {
      b = a;
      a = r;
   }

   return a;
}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_GCD_HPP
