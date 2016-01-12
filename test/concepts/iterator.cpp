// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace models {
  template <class...Ts>
  constexpr bool Same = ranges::Same<Ts...>();

  template <class R>
  constexpr bool Readable = ranges::Readable<R>();

  template <class W, class T>
  constexpr bool Writable = ranges::Writable<W, T>();

  template <class I>
  constexpr bool WeaklyIncrementable =
    ranges::WeaklyIncrementable<I>();

  template <class I>
  constexpr bool Incrementable = ranges::Incrementable<I>();

  template <class I>
  constexpr bool WeakIterator = ranges::WeakIterator<I>();

  template <class I>
  constexpr bool Iterator = ranges::Iterator<I>();

  template <class I>
  constexpr bool InputIterator = ranges::InputIterator<I>();

  template <class S, class I>
  constexpr bool Sentinel = ranges::IteratorRange<I, S>();

  template <class F, class...Is>
  constexpr bool IndirectCallable =
    ranges::IndirectCallable<F, Is...>();
}

namespace ns {
  template <class I>
  using difference_type_t = ranges::iterator_difference_t<I>;

  template <class I>
  using iterator_category_t = ranges::iterator_category_t<I>;

  template <class I>
  using reference_t = ranges::iterator_reference_t<I>;

  template <class I>
  using rvalue_reference_t = ranges::iterator_rvalue_reference_t<I>;

  template <class I>
  using value_type_t = ranges::iterator_value_t<I>;

  using ranges::value_type;
  using ranges::difference_type;
  using ranges::iterator_category;

  using ranges::input_iterator_tag;
  using ranges::forward_iterator_tag;
  using ranges::bidirectional_iterator_tag;
  using ranges::random_access_iterator_tag;
}

#elif VALIDATE_STL2
#include <stl2/iterator.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/core.hpp>

namespace ns = ::__stl2;
#endif

#include <cstddef>
#include <memory>
#include <type_traits>

#include <meta/meta.hpp>

#include "../simple_test.hpp"

namespace associated_type_test {
  struct A { using value_type = int; int& operator*() const; };
  struct B : A { using value_type = double; };
  struct C : A { using element_type = double; };
  struct D : A {
    using value_type = double;
    using element_type = char;
    using iterator_category = ns::forward_iterator_tag;
  };

  CONCEPT_ASSERT(models::Same<int&, ns::reference_t<int*>>);
  CONCEPT_ASSERT(models::Same<int&, ns::reference_t<int[]>>);
  CONCEPT_ASSERT(models::Same<int&, ns::reference_t<int[4]>>);
  CONCEPT_ASSERT(models::Same<int&, ns::reference_t<A>>);
  CONCEPT_ASSERT(models::Same<int&, ns::reference_t<B>>);
  CONCEPT_ASSERT(models::Same<int&, ns::reference_t<C>>);
  CONCEPT_ASSERT(models::Same<int&, ns::reference_t<D>>);
  CONCEPT_ASSERT(models::Same<const int&, ns::reference_t<const int*>>);

  CONCEPT_ASSERT(models::Same<int&&, ns::rvalue_reference_t<int*>>);
  CONCEPT_ASSERT(models::Same<int&&, ns::rvalue_reference_t<int[]>>);
  CONCEPT_ASSERT(models::Same<int&&, ns::rvalue_reference_t<int[4]>>);
  CONCEPT_ASSERT(models::Same<int&&, ns::rvalue_reference_t<A>>);
  CONCEPT_ASSERT(models::Same<int&&, ns::rvalue_reference_t<B>>);
  CONCEPT_ASSERT(models::Same<int&&, ns::rvalue_reference_t<C>>);
  CONCEPT_ASSERT(models::Same<int&&, ns::rvalue_reference_t<D>>);
  CONCEPT_ASSERT(models::Same<const int&&, ns::rvalue_reference_t<const int*>>);

  CONCEPT_ASSERT(models::Same<int, ns::value_type_t<int*>>);
  CONCEPT_ASSERT(models::Same<int, ns::value_type_t<int[]>>);
  CONCEPT_ASSERT(models::Same<int, ns::value_type_t<int[4]>>);
  CONCEPT_ASSERT(models::Same<int, ns::value_type_t<A>>);
  CONCEPT_ASSERT(models::Same<double, ns::value_type_t<B>>);
#if VALIDATE_STL2
  CONCEPT_ASSERT(models::Same<int, ns::value_type_t<C>>);
  CONCEPT_ASSERT(models::Same<double, ns::value_type_t<D>>);
#endif
  CONCEPT_ASSERT(models::Same<int, ns::value_type_t<const int*>>);
  CONCEPT_ASSERT(!meta::has_type<ns::value_type<void>>());
  CONCEPT_ASSERT(!meta::has_type<ns::value_type<void*>>());

  CONCEPT_ASSERT(models::Same<std::ptrdiff_t, ns::difference_type_t<int*>>);
  CONCEPT_ASSERT(models::Same<std::ptrdiff_t, ns::difference_type_t<int[]>>);
  CONCEPT_ASSERT(models::Same<std::ptrdiff_t, ns::difference_type_t<int[4]>>);

  CONCEPT_ASSERT(!meta::has_type<ns::difference_type<void>>());
  CONCEPT_ASSERT(!meta::has_type<ns::difference_type<void*>>());

  CONCEPT_ASSERT(models::Same<int, ns::difference_type_t<int>>);
  CONCEPT_ASSERT(models::Same<ns::iterator_category_t<D>, ns::forward_iterator_tag>);
#if VALIDATE_STL2
  CONCEPT_ASSERT(models::Same<ns::iterator_category_t<int*>, __stl2::ext::contiguous_iterator_tag>);
  CONCEPT_ASSERT(models::Same<ns::iterator_category_t<const int*>, __stl2::ext::contiguous_iterator_tag>);
#elif VALIDATE_RANGES
  CONCEPT_ASSERT(models::Same<ns::iterator_category_t<int*>, ns::random_access_iterator_tag>);
  CONCEPT_ASSERT(models::Same<ns::iterator_category_t<const int*>, ns::random_access_iterator_tag>);
#endif

  template <class T>
  struct derive_from : T {};

  template <class T, bool Derive>
  using iterator =
    meta::apply_list<
      meta::bind_front<
        meta::quote<std::iterator>,
        meta::if_c<Derive, derive_from<T>, T>>,
      meta::if_<
        std::is_same<T, std::output_iterator_tag>,
        meta::list<void, void, void, void>,
        meta::list<int>>>;

  template <class T, bool B, class U>
  using test = std::is_same<ns::iterator_category_t<iterator<T, B>>, U>;

  CONCEPT_ASSERT(!meta::has_type<ns::iterator_category<iterator<std::output_iterator_tag, false>>>());
  CONCEPT_ASSERT(!meta::has_type<ns::iterator_category<iterator<std::output_iterator_tag, true>>>());

  CONCEPT_ASSERT(test<std::input_iterator_tag, false, ns::input_iterator_tag>());
  CONCEPT_ASSERT(test<std::forward_iterator_tag, false, ns::forward_iterator_tag>());
  CONCEPT_ASSERT(test<std::bidirectional_iterator_tag, false, ns::bidirectional_iterator_tag>());
  CONCEPT_ASSERT(test<std::random_access_iterator_tag, false, ns::random_access_iterator_tag>());

  CONCEPT_ASSERT(test<std::input_iterator_tag, true, ns::input_iterator_tag>());
  CONCEPT_ASSERT(test<std::forward_iterator_tag, true, ns::forward_iterator_tag>());
  CONCEPT_ASSERT(test<std::bidirectional_iterator_tag, true, ns::bidirectional_iterator_tag>());
  CONCEPT_ASSERT(test<std::random_access_iterator_tag, true, ns::random_access_iterator_tag>());

  struct foo {};
  CONCEPT_ASSERT(test<foo, false, foo>());

  // Some sanity tests
  struct my_wonky_tag : std::random_access_iterator_tag, ns::random_access_iterator_tag {};
  struct my_wonky_tag2 : std::input_iterator_tag, ns::random_access_iterator_tag {};
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag, int>, my_wonky_tag>::value, "");
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag, int&>, my_wonky_tag>::value, "");
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag2, int>, my_wonky_tag2>::value, "");
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag2, int&>, my_wonky_tag2>::value, "");
  struct my_wonky_tag3 : ns::random_access_iterator_tag {};
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag3, int>, std::input_iterator_tag>::value, "");
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag3, int&>, std::random_access_iterator_tag>::value, "");
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<ns::input_iterator_tag, int>, std::input_iterator_tag>::value, "");
  static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<ns::input_iterator_tag, int&>, std::input_iterator_tag>::value, "");
} // namespace associated_type_test

namespace readable_test {
  struct A {
    int operator*() const;
    using value_type = int;
  };

  CONCEPT_ASSERT(!models::Readable<void>);
  CONCEPT_ASSERT(!models::Readable<void*>);
  CONCEPT_ASSERT(models::Readable<int*>);
  CONCEPT_ASSERT(models::Readable<const int*>);
  CONCEPT_ASSERT(models::Readable<A>);
  CONCEPT_ASSERT(models::Same<ns::value_type_t<A>,int>);
}

namespace writable_test {
  struct A {
    int& operator*() const;
  };

  CONCEPT_ASSERT(models::Writable<std::unique_ptr<int>*, std::unique_ptr<int>&&>);
  CONCEPT_ASSERT(!models::Writable<std::unique_ptr<int>*, std::unique_ptr<int>&>);
  CONCEPT_ASSERT(!models::Writable<void, int>);
  CONCEPT_ASSERT(!models::Writable<void*, void>);
  CONCEPT_ASSERT(models::Writable<int*, int>);
  CONCEPT_ASSERT(models::Writable<int*, int&>);
  CONCEPT_ASSERT(models::Writable<int*, const int&>);
  CONCEPT_ASSERT(models::Writable<int*, const int>);
  CONCEPT_ASSERT(!models::Writable<const int*, int>);
  CONCEPT_ASSERT(models::Writable<A, int>);
  CONCEPT_ASSERT(models::Writable<A, const int&>);
  CONCEPT_ASSERT(models::Writable<A, double>);
  CONCEPT_ASSERT(models::Writable<A, const double&>);
} // namespace writable_test

CONCEPT_ASSERT(models::WeaklyIncrementable<int>);
CONCEPT_ASSERT(models::WeaklyIncrementable<unsigned int>);
CONCEPT_ASSERT(!models::WeaklyIncrementable<void>);
CONCEPT_ASSERT(models::WeaklyIncrementable<int*>);
CONCEPT_ASSERT(models::WeaklyIncrementable<const int*>);

CONCEPT_ASSERT(models::Incrementable<int>);
CONCEPT_ASSERT(models::Incrementable<unsigned int>);
CONCEPT_ASSERT(!models::Incrementable<void>);
CONCEPT_ASSERT(models::Incrementable<int*>);
CONCEPT_ASSERT(models::Incrementable<const int*>);

namespace iterator_sentinel_test {
  struct A {
    using difference_type = signed char;
    using iterator_category = ns::input_iterator_tag;
    using value_type = double;

    A& operator++();
    A operator++(int);
    double operator*() const;

    bool operator == (const A&) const;
    bool operator != (const A&) const;
  };

  CONCEPT_ASSERT(models::Iterator<int*>);
  CONCEPT_ASSERT(models::Iterator<const int*>);
  CONCEPT_ASSERT(!models::Iterator<void*>);
  CONCEPT_ASSERT(models::Iterator<A>);
  CONCEPT_ASSERT(models::InputIterator<A>);

  CONCEPT_ASSERT(models::Iterator<int*>);
  CONCEPT_ASSERT(models::Sentinel<int*, int*>);
  CONCEPT_ASSERT(models::Sentinel<const int*, const int*>);
  CONCEPT_ASSERT(models::Sentinel<const int*, int*>);
  CONCEPT_ASSERT(!models::Sentinel<void*, void*>);
  CONCEPT_ASSERT(models::Sentinel<A, A>);
} // namespace iterator_sentinel_test

namespace indirectly_callable_test {
  CONCEPT_ASSERT(models::IndirectCallable<std::plus<int>, int*, int*>);
}

int main() {
  return ::test_result();
}
