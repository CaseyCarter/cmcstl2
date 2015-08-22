#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <cassert>
#include <initializer_list>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/detail/iterator/move_iterator.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>

namespace stl2 { inline namespace v1 {
using std::iterator;

template <CopyConstructible T>
class dangling {
public:
  dangling() requires DefaultConstructible<T>()
    : value{}
  { }
  dangling(T t)
    : value(move(t))
  { }
  T get_unsafe() const {
    return value;
  }
private:
  T value;
};

template <Range R>
using safe_iterator_t =
  meta::if_<std::is_lvalue_reference<R>, IteratorType<R>, dangling<IteratorType<R>>>;
}}

#endif
