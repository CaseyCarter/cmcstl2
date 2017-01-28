// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//  Copyright Casey Carter 2017
//  Copyright Christopher Di Bella 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_TEST_NUMERIC_TEST_INTERFACE_HPP
#define STL2_TEST_NUMERIC_TEST_INTERFACE_HPP

#include "../simple_test.hpp"
#include <stl2/detail/concepts/container.hpp>
#include <stl2/detail/fwd.hpp>
#include <utility>
#include <vector>

template <typename T, typename... Ts>
struct First_of {
   using type = T;
};

template <typename... Ts>
using First_of_t = typename First_of<Ts...>::type;

template <__stl2::ext::Container C,
          __stl2::RegularInvocable<__stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>> F1,
          __stl2::RegularInvocable<__stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>> F2,
          __stl2::RegularInvocable<C&&, __stl2::iterator_t<C>> F3>
void CHECK_algorithm(const C& v, F1 stdcall, F2 stl2_call, F3 range_call) noexcept
{
   auto s = C(v.size());
   __stl2::invoke(__stl2::move(stdcall), v.begin(), v.end(), __stl2::begin(s));

   auto r = C(v.size());
   auto result = __stl2::invoke(__stl2::move(stl2_call), v.begin(), v.end(), r.begin());
   CHECK(s == r);
   CHECK(result.in() == v.end());
   CHECK(result.out() == r.end());

   r = C(v.size());
   result = __stl2::invoke(__stl2::move(range_call), v, r.begin());
   CHECK(s == r);
   CHECK(result.in() == v.end());
   CHECK(result.out() == r.end());
}

template <__stl2::ext::Container C,
          __stl2::RegularInvocable<__stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>> F1,
          __stl2::RegularInvocable<__stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>> F2,
          __stl2::RegularInvocable<C&&, __stl2::iterator_t<C>> F3>
void CHECK_empty(F1&& stdcall, F2&& stl2_call, F3&& range_call) noexcept
{
   CHECK_algorithm(C{},
                   __stl2::forward<F1>(stdcall),
                   __stl2::forward<F2>(stl2_call),
                   __stl2::forward<F3>(range_call));
}

template <__stl2::ext::Container C,
          __stl2::RegularInvocable<__stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>> F1,
          __stl2::RegularInvocable<__stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>> F2,
          __stl2::RegularInvocable<C&&, __stl2::iterator_t<C>> F3>
void CHECK_single_element(F1&& stdcall, F2&& stl2_call, F3&& range_call) noexcept
{
   CHECK_algorithm(C{__stl2::value_type_t<C>{}},
                   __stl2::forward<F1>(stdcall),
                   __stl2::forward<F2>(stl2_call),
                   __stl2::forward<F3>(range_call));
}

template <template <typename Z, typename = std::allocator<Z>> class T,
          class F1, class F2, class F3,
          __stl2::Regular... Args,
          __stl2::ext::Container C = T<First_of_t<Args...>>>
requires
   __stl2::Same<Args...>() &&
   __stl2::RegularInvocable<F1, __stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>>() &&
   __stl2::RegularInvocable<F2, __stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>>() &&
   __stl2::RegularInvocable<F3, C&&, __stl2::iterator_t<C>>()
void CHECK_many(F1&& stdcall, F2&& stl2_call, F3&& range_call, Args&&... args) noexcept
{
   CHECK_algorithm(C{__stl2::forward<Args>(args)...},
                   __stl2::forward<F1>(stdcall),
                   __stl2::forward<F2>(stl2_call),
                   __stl2::forward<F3>(range_call));
}

template <__stl2::Regular T>
struct Minus_pair {
   T operator()(const T& a, const T& b) const noexcept
   {
      return {b.first - a.first, b.second - a.second};
   }
};

template <__stl2::ext::Container C = std::vector<std::pair<int, int>>,
          class Op = Minus_pair<__stl2::value_type_t<C>>,
          __stl2::RegularInvocable<__stl2::iterator_t<C>, __stl2::sentinel_t<C>, __stl2::iterator_t<C>, Op> F1,
          __stl2::RegularInvocable<C&&, __stl2::iterator_t<C>, Op> F2>
void CHECK_custom_op(F1 stdcall, F2 range_call) noexcept
{
   const auto c = C{{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 9}};
   auto s = C(c.size());
   __stl2::invoke(__stl2::move(stdcall), c.begin(), c.end(), __stl2::begin(s), Op{});

   auto r = C(c.size());
   auto result = __stl2::invoke(__stl2::move(range_call), c, r.begin(), Op{});
   CHECK(s == r);
   CHECK(result.in() == c.end());
   CHECK(result.out() == r.end());
}

void CHECK_projection() noexcept;
void CHECK_insert_iterator() noexcept;
#endif // STL2_TEST_NUMERIC_TEST_INTERFACE_HPP
