#ifndef STL2_DETAIL_NUMERIC_GCD_HPP
#define STL2_DETAIL_NUMERIC_GCD_HPP
#include <stl2/tuple.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/algorithm/minmax.hpp>
#include <stl2/detail/concepts/number.hpp>

STL2_OPEN_NAMESPACE {
template <class M, class N, class __CommonType = common_type_t<M, N>>
   requires
      models::Same<__CommonType, common_type_t<M, N>> &&
      models::Number<__CommonType, M, N> &&
      requires(__CommonType t) {
         {t % t}  -> __CommonType;
      }
constexpr __CommonType gcd(M m, N n)
{
   constexpr auto zero = __CommonType{0};
   // TODO: replace with structured bindings when compiler support exists
   __CommonType a{static_cast<__CommonType>(m < 0 ? -m : m)};
   __CommonType b{static_cast<__CommonType>(n < 0 ? -n : n)};
   {
      auto p = __stl2::minmax(a, b);
      a = p.min();
      b = p.max();
   }
   // end TODO

   if (a == zero || b == zero)
      return zero;

   // b = ac + r
   for (auto r = b % a; r != zero; r = b % a) {
      b = a;
      a = r;
   }

   return a;
}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_GCD_HPP
