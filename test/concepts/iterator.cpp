// -*- compile-command: "(cd ~/cmcstl2/build && make concepts.iterator && ./test/concepts/concepts.iterator)" -*-

#include <stl2/concepts/core.hpp>
#include <stl2/concepts/iterator.hpp>

#include <cstddef>
#include <memory>
#include <type_traits>

#include <meta/meta.hpp>

#include "../simple_test.hpp"

namespace associated_type_test {
using stl2::ext::models::same;
using stl2::DifferenceType;
using stl2::IteratorCategory;
using stl2::ReferenceType;
using stl2::RvalueReferenceType;
using stl2::ValueType;

struct A { int& operator*() const; };
struct B : A { using value_type = double; };
struct C : A { using element_type = double; };
struct D : A {
  using value_type = double;
  using element_type = char;
  using iterator_category = stl2::forward_iterator_tag;
};

static_assert(same<int&, ReferenceType<int*>>(), "");
static_assert(same<int&, ReferenceType<int[]>>(), "");
static_assert(same<int&, ReferenceType<int[4]>>(), "");
static_assert(same<int&, ReferenceType<A>>(), "");
static_assert(same<int&, ReferenceType<B>>(), "");
static_assert(same<int&, ReferenceType<C>>(), "");
static_assert(same<int&, ReferenceType<D>>(), "");
static_assert(same<const int&, ReferenceType<const int*>>(), "");

static_assert(same<int&&, RvalueReferenceType<int*>>(), "");
static_assert(same<int&&, RvalueReferenceType<int[]>>(), "");
static_assert(same<int&&, RvalueReferenceType<int[4]>>(), "");
static_assert(same<int&&, RvalueReferenceType<A>>(), "");
static_assert(same<int&&, RvalueReferenceType<B>>(), "");
static_assert(same<int&&, RvalueReferenceType<C>>(), "");
static_assert(same<int&&, RvalueReferenceType<D>>(), "");
static_assert(same<const int&&, RvalueReferenceType<const int*>>(), "");

static_assert(same<int, ValueType<int*>>(), "");
static_assert(same<int, ValueType<int[]>>(), "");
static_assert(same<int, ValueType<int[4]>>(), "");
static_assert(same<int, ValueType<A>>(), "");
static_assert(same<double, ValueType<B>>(), "");
static_assert(same<double, ValueType<C>>(), "");
static_assert(same<double, ValueType<D>>(), "");
static_assert(same<int, ValueType<const int*>>(), "");
static_assert(!meta::has_type<stl2::value_type<void>>(), "");
static_assert(!meta::has_type<stl2::value_type<void*>>(), "");

static_assert(same<std::ptrdiff_t, DifferenceType<int*>>(), "");
static_assert(same<std::ptrdiff_t, DifferenceType<int[]>>(), "");
static_assert(same<std::ptrdiff_t, DifferenceType<int[4]>>(), "");
static_assert(same<std::ptrdiff_t, DifferenceType<std::nullptr_t>>(), "");

static_assert(!meta::has_type<stl2::difference_type<void>>(), "");
static_assert(!meta::has_type<stl2::difference_type<void*>>(), "");

static_assert(same<int, DifferenceType<int>>(), "");

static_assert(same<IteratorCategory<int*>, stl2::ext::contiguous_iterator_tag>(), "");
static_assert(same<IteratorCategory<const int*>, stl2::ext::contiguous_iterator_tag>(), "");
static_assert(same<IteratorCategory<D>, stl2::forward_iterator_tag>(), "");
} // namespace associated_type_test

namespace readable_test {
using stl2::ext::models::readable;
using stl2::ValueType;
using stl2::ext::models::same;

struct A {
  int operator*() const;
};

static_assert(!readable<void>(), "");
static_assert(!readable<void*>(), "");
static_assert(readable<int*>(), "");
static_assert(readable<const int*>(), "");
static_assert(readable<A>(), "");
static_assert(same<ValueType<A>,int>(), "");
}

namespace writable_test {
using stl2::ext::models::move_writable;
using stl2::ext::models::writable;

struct A {
  int& operator*() const;
};

static_assert(move_writable<std::unique_ptr<int>*, std::unique_ptr<int>>(), "");
static_assert(!writable<std::unique_ptr<int>*, std::unique_ptr<int>>(), "");
static_assert(!writable<void, int>(), "");
static_assert(!writable<void*, void>(), "");
static_assert(writable<int*, int>(), "");
static_assert(writable<int*, int&>(), "");
static_assert(writable<int*, const int&>(), "");
static_assert(writable<int*, const int>(), "");
static_assert(!writable<const int*, int>(), "");
static_assert(writable<A, int>(), "");
static_assert(writable<A, double>(), "");
} // namespace writable_test

namespace weakly_incrementable_test {
using stl2::ext::models::weakly_incrementable;

static_assert(weakly_incrementable<int>(), "");
static_assert(weakly_incrementable<unsigned int>(), "");
static_assert(!weakly_incrementable<void>(), "");
static_assert(weakly_incrementable<int*>(), "");
static_assert(weakly_incrementable<const int*>(), "");
} // namespace weakly_incrementable_test

namespace incrementable_test {
using stl2::ext::models::incrementable;

static_assert(incrementable<int>(), "");
static_assert(incrementable<unsigned int>(), "");
static_assert(!incrementable<void>(), "");
static_assert(incrementable<int*>(), "");
static_assert(incrementable<const int*>(), "");
} // namespace incrementable_test

namespace weak_iterator_test {
namespace models = stl2::ext::models;
using stl2::RvalueReferenceType;

struct A {
  using difference_type = signed char;
  A& operator++();
  A operator++(int);
  double operator*() const;
};

static_assert(models::weak_iterator<int*>(), "");
static_assert(models::weak_iterator<const int*>(), "");
static_assert(!models::weak_iterator<void*>(), "");
static_assert(models::weak_iterator<A>(), "");
static_assert(models::same<RvalueReferenceType<A>, double>(), "");
} // namespace weak_iterator_test

namespace iterator_sentinel_test {
namespace models = stl2::ext::models;

struct A {
  using difference_type = signed char;
  using iterator_category = stl2::input_iterator_tag;
  A& operator++();
  A operator++(int);
  double operator*() const;

  bool operator == (const A&) const;
  bool operator != (const A&) const;
};

static_assert(models::iterator<int*>(), "");
static_assert(models::iterator<const int*>(), "");
static_assert(!models::iterator<void*>(), "");
static_assert(models::iterator<A>(), "");
  //static_assert(models::input_iterator<A>(), "");

static_assert(models::iterator<int*>(), "");
static_assert(models::regular<int*>(), "");
static_assert(models::equality_comparable<int*, int*>(), "");

#if 0 // FIXME: compiling models::sentinel explodes memory
static_assert(models::sentinel<int*, int*>(), "");
static_assert(models::sentinel<const int*, const int*>(), "");
static_assert(models::sentinel<const int*, int*>(), "");
static_assert(!models::sentinel<void*, void*>(), "");
static_assert(models::sentinel<A, A>(), "");
#endif
} // namespace iterator_sentinel_test

int main() {
  return ::test_result();
}
