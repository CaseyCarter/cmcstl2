#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace models {
//template <class...Ts>
//using same = ranges::Same<Ts...>;
template <class T, class U>
using same = ranges::Same<T, U>;

template <class R>
using readable = ranges::Readable<R>;

template <class W, class T>
using move_writable = ranges::MoveWritable<W, T>;

template <class W, class T>
using writable = ranges::Writable<W, T>;

template <class I>
using weakly_incrementable = ranges::WeaklyIncrementable<I>;

template <class I>
using incrementable = ranges::Incrementable<I>;

template <class I>
using weak_iterator = ranges::WeakIterator<I>;

template <class I>
using iterator = ranges::Iterator<I>;

template <class I>
using input_iterator = ranges::InputIterator<I>;

template <class S, class I>
using sentinel = ranges::IteratorRange<I, S>;
}

namespace ns {
template <class I>
using DifferenceType = ranges::iterator_difference_t<I>;

template <class I>
using IteratorCategory = ranges::iterator_category_t<I>;

template <class I>
using ReferenceType = ranges::iterator_reference_t<I>;

template <class I>
using RvalueReferenceType = ranges::iterator_rvalue_reference_t<I>;

template <class I>
using ValueType = ranges::iterator_value_t<I>;

using ranges::value_type;
using ranges::difference_type;
using ranges::iterator_category;

using ranges::input_iterator_tag;
using ranges::forward_iterator_tag;
using ranges::bidirectional_iterator_tag;
using ranges::random_access_iterator_tag;
}

#elif VALIDATE_STL2
#include <stl2/detail/concepts/core.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/concepts/callable.hpp>

namespace ns {
using stl2::DifferenceType;
using stl2::IteratorCategory;
using stl2::ReferenceType;
using stl2::RvalueReferenceType;
using stl2::ValueType;

using stl2::value_type;
using stl2::difference_type;
using stl2::iterator_category;

using stl2::input_iterator_tag;
using stl2::forward_iterator_tag;
using stl2::bidirectional_iterator_tag;
using stl2::random_access_iterator_tag;
}
#endif

#include <cstddef>
#include <memory>
#include <type_traits>

#include <meta/meta.hpp>

#include "../simple_test.hpp"

namespace associated_type_test {
struct A { int& operator*() const; };
struct B : A { using value_type = double; };
struct C : A { using element_type = double; };
struct D : A {
  using value_type = double;
  using element_type = char;
  using iterator_category = ns::forward_iterator_tag;
};

CONCEPT_ASSERT(models::same<int&, ns::ReferenceType<int*>>());
CONCEPT_ASSERT(models::same<int&, ns::ReferenceType<int[]>>());
CONCEPT_ASSERT(models::same<int&, ns::ReferenceType<int[4]>>());
CONCEPT_ASSERT(models::same<int&, ns::ReferenceType<A>>());
CONCEPT_ASSERT(models::same<int&, ns::ReferenceType<B>>());
CONCEPT_ASSERT(models::same<int&, ns::ReferenceType<C>>());
CONCEPT_ASSERT(models::same<int&, ns::ReferenceType<D>>());
CONCEPT_ASSERT(models::same<const int&, ns::ReferenceType<const int*>>());

CONCEPT_ASSERT(models::same<int&&, ns::RvalueReferenceType<int*>>());
CONCEPT_ASSERT(models::same<int&&, ns::RvalueReferenceType<int[]>>());
CONCEPT_ASSERT(models::same<int&&, ns::RvalueReferenceType<int[4]>>());
CONCEPT_ASSERT(models::same<int&&, ns::RvalueReferenceType<A>>());
CONCEPT_ASSERT(models::same<int&&, ns::RvalueReferenceType<B>>());
CONCEPT_ASSERT(models::same<int&&, ns::RvalueReferenceType<C>>());
CONCEPT_ASSERT(models::same<int&&, ns::RvalueReferenceType<D>>());
CONCEPT_ASSERT(models::same<const int&&, ns::RvalueReferenceType<const int*>>());

CONCEPT_ASSERT(models::same<int, ns::ValueType<int*>>());
CONCEPT_ASSERT(models::same<int, ns::ValueType<int[]>>());
CONCEPT_ASSERT(models::same<int, ns::ValueType<int[4]>>());
CONCEPT_ASSERT(models::same<double, ns::ValueType<B>>());
CONCEPT_ASSERT(models::same<double, ns::ValueType<C>>());
#if VALIDATE_STL2
CONCEPT_ASSERT(models::same<int, ns::ValueType<A>>());
CONCEPT_ASSERT(models::same<double, ns::ValueType<D>>());
#endif
CONCEPT_ASSERT(models::same<int, ns::ValueType<const int*>>());
CONCEPT_ASSERT(!meta::has_type<ns::value_type<void>>());
CONCEPT_ASSERT(!meta::has_type<ns::value_type<void*>>());

CONCEPT_ASSERT(models::same<std::ptrdiff_t, ns::DifferenceType<int*>>());
CONCEPT_ASSERT(models::same<std::ptrdiff_t, ns::DifferenceType<int[]>>());
CONCEPT_ASSERT(models::same<std::ptrdiff_t, ns::DifferenceType<int[4]>>());
CONCEPT_ASSERT(models::same<std::ptrdiff_t, ns::DifferenceType<std::nullptr_t>>());

CONCEPT_ASSERT(!meta::has_type<ns::difference_type<void>>());
CONCEPT_ASSERT(!meta::has_type<ns::difference_type<void*>>());

CONCEPT_ASSERT(models::same<int, ns::DifferenceType<int>>());
CONCEPT_ASSERT(models::same<ns::IteratorCategory<D>, ns::forward_iterator_tag>());
#if VALIDATE_STL2
CONCEPT_ASSERT(models::same<ns::IteratorCategory<int*>, stl2::ext::contiguous_iterator_tag>());
CONCEPT_ASSERT(models::same<ns::IteratorCategory<const int*>, stl2::ext::contiguous_iterator_tag>());
#elif VALIDATE_RANGES
CONCEPT_ASSERT(models::same<ns::IteratorCategory<int*>, ns::random_access_iterator_tag>());
CONCEPT_ASSERT(models::same<ns::IteratorCategory<const int*>, ns::random_access_iterator_tag>());
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
using test = std::is_same<ns::IteratorCategory<iterator<T, B>>, U>;

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
} // namespace associated_type_test

namespace readable_test {
struct A {
  int operator*() const;
  using value_type = int;
};

CONCEPT_ASSERT(!models::readable<void>());
CONCEPT_ASSERT(!models::readable<void*>());
CONCEPT_ASSERT(models::readable<int*>());
CONCEPT_ASSERT(models::readable<const int*>());
CONCEPT_ASSERT(models::readable<A>());
CONCEPT_ASSERT(models::same<ns::ValueType<A>,int>());
}

namespace writable_test {
struct A {
  int& operator*() const;
};

CONCEPT_ASSERT(models::move_writable<std::unique_ptr<int>*, std::unique_ptr<int>>());
CONCEPT_ASSERT(!models::writable<std::unique_ptr<int>*, std::unique_ptr<int>>());
CONCEPT_ASSERT(!models::writable<void, int>());
CONCEPT_ASSERT(!models::writable<void*, void>());
CONCEPT_ASSERT(models::writable<int*, int>());
CONCEPT_ASSERT(models::writable<int*, int&>());
CONCEPT_ASSERT(models::writable<int*, const int&>());
CONCEPT_ASSERT(models::writable<int*, const int>());
CONCEPT_ASSERT(!models::writable<const int*, int>());
CONCEPT_ASSERT(models::writable<A, int>());
CONCEPT_ASSERT(models::writable<A, double>());
} // namespace writable_test

CONCEPT_ASSERT(models::weakly_incrementable<int>());
CONCEPT_ASSERT(models::weakly_incrementable<unsigned int>());
CONCEPT_ASSERT(!models::weakly_incrementable<void>());
CONCEPT_ASSERT(models::weakly_incrementable<int*>());
CONCEPT_ASSERT(models::weakly_incrementable<const int*>());

CONCEPT_ASSERT(models::incrementable<int>());
CONCEPT_ASSERT(models::incrementable<unsigned int>());
CONCEPT_ASSERT(!models::incrementable<void>());
CONCEPT_ASSERT(models::incrementable<int*>());
CONCEPT_ASSERT(models::incrementable<const int*>());

namespace weak_iterator_test {
struct A {
  using difference_type = signed char;
  using value_type = double;

  A& operator++();
  A operator++(int);
  double operator*() const;
};

CONCEPT_ASSERT(models::weak_iterator<int*>());
CONCEPT_ASSERT(models::weak_iterator<const int*>());
CONCEPT_ASSERT(!models::weak_iterator<void*>());
CONCEPT_ASSERT(models::weak_iterator<A>());
CONCEPT_ASSERT(models::same<ns::RvalueReferenceType<A>, double>());
} // namespace weak_iterator_test

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

CONCEPT_ASSERT(models::iterator<int*>());
CONCEPT_ASSERT(models::iterator<const int*>());
CONCEPT_ASSERT(!models::iterator<void*>());
CONCEPT_ASSERT(models::iterator<A>());
CONCEPT_ASSERT(models::input_iterator<A>());

CONCEPT_ASSERT(models::iterator<int*>());
CONCEPT_ASSERT(models::sentinel<int*, int*>());
CONCEPT_ASSERT(models::sentinel<const int*, const int*>());
CONCEPT_ASSERT(models::sentinel<const int*, int*>());
CONCEPT_ASSERT(!models::sentinel<void*, void*>());
CONCEPT_ASSERT(models::sentinel<A, A>());
} // namespace iterator_sentinel_test

namespace indirectly_callable_test {
CONCEPT_ASSERT(models::indirect_callable<std::plus<int>, int*, int*>());
}

int main() {
  return ::test_result();
}
