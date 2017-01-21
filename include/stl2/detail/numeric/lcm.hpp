#ifndef STL2_DETAIL_NUMERIC_LCM_HPP
#define STL2_DETAIL_NUMERIC_LCM_HPP

#include <stl2/tuple.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/algorithm/minmax.hpp>
#include <stl2/detail/concepts/number.hpp>
#include <stl2/detail/numeric/gcd.hpp>

STL2_OPEN_NAMESPACE {
template <class M, class N, class T = common_type_t<M, N>>
   requires
      models::Same<T, common_type_t<M, N>> &&
		models::Number<T, M, N> &&
		requires(T t) {
			{t % t}  -> T;
		}
constexpr T lcm(M m, N n)
{
   // TODO: replace with structured bindings when compiler support exists
   const auto a = static_cast<T>(m < 0 ? -m : m);
   const auto b = static_cast<T>(n < 0 ? -n : n);
   const auto p = __stl2::minmax(a, b);
   return (p.min() / __stl2::gcd(a, b)) * p.max();
   // end TODO
}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_LCM_HPP
