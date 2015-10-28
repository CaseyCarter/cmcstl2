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
#ifndef STL2_DETAIL_ITERATOR_CONCEPTS_HPP
#define STL2_DETAIL_ITERATOR_CONCEPTS_HPP

#include <functional>
#include <iterator>

#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/increment.hpp>

///////////////////////////////////////////////////////////////////////////
// Iterator concepts [iterator.requirements]
//
STL2_OPEN_NAMESPACE {
  namespace detail {
    template <class T>
    constexpr bool __dereferenceable = false;
    template <class T>
      requires requires (T& t) {
        STL2_DEDUCE_AUTO_REF_REF(*t);
      }
    constexpr bool __dereferenceable<T> = true;

    template <class T>
    concept bool Dereferenceable = __dereferenceable<T&>;
  }

  ///////////////////////////////////////////////////////////////////////////
  // reference_t [iterator.assoc]
  // Not to spec: forbids void.
  //
  detail::Dereferenceable{R}
  using reference_t =
    decltype(*declval<R&>());

  ///////////////////////////////////////////////////////////////////////////
  // iter_move [Extension]
  // From the proxy iterator work.
  //
  namespace __iter_move {
    detail::Dereferenceable{R}
    using __iter_move_t =
      meta::if_<
        is_reference<reference_t<R>>,
        remove_reference_t<reference_t<R>> &&,
        decay_t<reference_t<R>>>;

    template <class R, detail::Dereferenceable _R = remove_reference_t<R>>
    constexpr __iter_move_t<_R> iter_move(R&& r)
      noexcept(noexcept(__iter_move_t<_R>(__stl2::move(*r)))) {
      return __stl2::move(*r);
    }

    template <class R>
    constexpr bool can_iter_move = false;
    template <class R>
      requires requires (R&& r) { { iter_move((R&&)r) } -> auto&&; }
    constexpr bool can_iter_move<R> = true;

    struct fn {
      template <class R>
        requires can_iter_move<R>
      constexpr decltype(auto) operator()(R&& r) const
      STL2_NOEXCEPT_RETURN(
        iter_move(__stl2::forward<R>(r))
      )
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& iter_move = detail::static_const<__iter_move::fn>::value;
  }

  ///////////////////////////////////////////////////////////////////////////
  // rvalue_reference_t [Extension]
  // From the proxy iterator work.
  //
  detail::Dereferenceable{R}
  using rvalue_reference_t =
    decltype(__stl2::iter_move(declval<R&>()));

  ///////////////////////////////////////////////////////////////////////////
  // value_type [readable.iterators]
  // 20150715: Not to spec for various reasons:
  // * Resolves the ambiguity for a class with both members value_type
  //   and element_type in favor of value_type.
  // * Defaults to decay_t<reference_t<T>> when neither value_type
  //   nor element_type are present.
  //
  namespace detail {
    template <class T>
    constexpr bool IsValueType = false;
    template <class T>
      requires _Decayed<T> && _IsNot<T, is_void>
    constexpr bool IsValueType<T> = true;

    template <class T>
    concept bool MemberValueType =
      requires { typename T::value_type; };

    template <class T>
    concept bool MemberElementType =
      requires { typename T::element_type; };
  }

  template <class>
  struct value_type {};

  detail::MemberValueType{T}
  struct value_type<T> {
    using type = typename T::value_type;
  };

  template <detail::MemberElementType T>
    requires !detail::MemberValueType<T>
  struct value_type<T> {
    using type = typename T::element_type;
  };

  template <detail::Dereferenceable T>
    requires !(detail::MemberElementType<T> || detail::MemberValueType<T>)
  struct value_type<T> {
    using type = decay_t<reference_t<T>>;
  };

  ///////////////////////////////////////////////////////////////////////////
  // value_type_t [readable.iterators]
  // 20150715: Not to spec.
  // * Strips cv-qualifiers before evaluating value_type. Someone out
  //   there may think it's a good idea to have T, const T, and volatile T
  //   be iterators with differing value types - I think they are insane.
  // * Requires value_type_t to actually be a value type, i.e., non-void
  //   and Same<T, decay_t<T>>. I don't think generic code can reasonably
  //   be expected to work with an iterator whose value_type is a function
  //   or array.
  // The end result is that the logic is mostly in value_type_t, since
  // value_type is subject to the depradations of user specialization.
  //
  template <class T>
    requires detail::IsValueType<meta::_t<value_type<remove_cv_t<T>>>>
  using value_type_t =
    meta::_t<value_type<remove_cv_t<T>>>;

  ///////////////////////////////////////////////////////////////////////////
  // Readable [readable.iterators]
  // Not to spec: Additional requirements from the proxy iterator work.
  //
  template <class I>
  constexpr bool __readable = false;
  template <class I>
    requires requires (const I& i) {
      // Associated types
      typename value_type_t<I>;
      typename reference_t<I>;
      typename rvalue_reference_t<I>;

      // Valid expressions
      STL2_EXACT_TYPE_CONSTRAINT(*i, reference_t<I>);
      STL2_EXACT_TYPE_CONSTRAINT(__stl2::iter_move(i), rvalue_reference_t<I>);
    }
  constexpr bool __readable<I> = true;

  template <class I>
  concept bool Readable() {
    return detail::Dereferenceable<I> &&
      Movable<I>() &&
      DefaultConstructible<I>() &&
      __readable<I> &&
      // Relationships between associated types
      CommonReference<reference_t<I>, value_type_t<I>&>() &&
      CommonReference<reference_t<I>, rvalue_reference_t<I>>() &&
      CommonReference<rvalue_reference_t<I>, const value_type_t<I>&>() &&
      // Extra sanity checks (not strictly needed)
      Same<
        CommonReferenceType<reference_t<I>, value_type_t<I>>,
        value_type_t<I>>() &&
      Same<
        CommonReferenceType<rvalue_reference_t<I>, value_type_t<I>>,
        value_type_t<I>>();
  }

  namespace models {
    template <class>
    constexpr bool Readable = false;
    __stl2::Readable{I}
    constexpr bool Readable<I> = true;

    // Readable<Is>() && ...
    template <class...Is>
    constexpr bool AllReadable = false;
    template <__stl2::Readable...Is>
    constexpr bool AllReadable<Is...> = true;
  }

  // A generally useful dependent type
  Readable{I}
  using iter_common_reference_t =
    common_reference_t<reference_t<I>, value_type_t<I>&>;

  ///////////////////////////////////////////////////////////////////////////
  // MoveWritable [movewritable.iterators]
  // Not to spec: Additional requirements from the proxy iterator work.
  //
  template <class Out, class T>
  constexpr bool __move_writable = false;
  template <class Out, class T>
    requires requires (Out& o, T&& t) {
      *o = (T&&)t; // Maybe void(*o = (T&&)t) ?
    }
  constexpr bool __move_writable<Out, T> = true;

  template <class Out, class T>
  concept bool MoveWritable() {
    return detail::Dereferenceable<Out> &&
      Movable<Out>() &&
      DefaultConstructible<Out>() &&
      __move_writable<Out, T>;
  }

  namespace models {
    template <class, class>
    constexpr bool MoveWritable = false;
    __stl2::MoveWritable{Out, T}
    constexpr bool MoveWritable<Out, T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Writable [writable.iterators]
  //
  template <class Out, class T>
  constexpr bool __writable = false;
  template <class Out, class T>
    requires requires (Out& o, const T& t) {
      *o = t; // Maybe void(*o = t) ?
    }
  constexpr bool __writable<Out, T> = true;

  template <class Out, class T>
  concept bool Writable() {
    return MoveWritable<Out, T>() &&
      __writable<Out, T>;
  }

  namespace models {
    template <class, class>
    constexpr bool Writable = false;
    __stl2::Writable{Out, T}
    constexpr bool Writable<Out, T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // IndirectlyMovable [indirectlymovable.iterators]
  // Not to spec: Additional requirements from the proxy iterator work.
  //
  template <class In, class Out>
  concept bool IndirectlyMovable() {
    return Readable<In>() && Movable<value_type_t<In>>() &&
      Constructible<value_type_t<In>, rvalue_reference_t<In>>() &&
      Assignable<value_type_t<In>&, rvalue_reference_t<In>>() &&
      MoveWritable<Out, rvalue_reference_t<In>>() &&
      MoveWritable<Out, value_type_t<In>>();
  }

  namespace models {
    template <class, class>
    constexpr bool IndirectlyMovable = false;
    __stl2::IndirectlyMovable{In, Out}
    constexpr bool IndirectlyMovable<In, Out> = true;
  }

  template <class In, class Out>
  constexpr bool is_nothrow_indirectly_movable_v = false;

  IndirectlyMovable{In, Out}
  constexpr bool is_nothrow_indirectly_movable_v<In, Out> =
    is_nothrow_constructible<value_type_t<In>, rvalue_reference_t<In>>::value &&
    is_nothrow_assignable<value_type_t<In> &, rvalue_reference_t<In>>::value &&
    is_nothrow_assignable<reference_t<Out>, rvalue_reference_t<In>>::value &&
    is_nothrow_assignable<reference_t<Out>, value_type_t<In>>::value;

  template <class In, class Out>
  using is_nothrow_indirectly_movable_t =
    meta::bool_<is_nothrow_indirectly_movable_v<In, Out>>;

  template <class In, class Out>
  struct is_nothrow_indirectly_movable :
    is_nothrow_indirectly_movable_t<In, Out> {};

  ///////////////////////////////////////////////////////////////////////////
  // IndirectlyCopyable [indirectlycopyable.iterators]
  // Not to spec: Additional requirements from the proxy iterator work.
  //
  template <class In, class Out>
  concept bool IndirectlyCopyable() {
    return IndirectlyMovable<In, Out>() && Copyable<value_type_t<In>>() &&
      Constructible<value_type_t<In>, reference_t<In>>() &&
      Assignable<value_type_t<In>&, reference_t<In>>() &&
      Writable<Out, reference_t<In>>() &&
      Writable<Out, value_type_t<In>>();
  }

  namespace models {
    template <class, class>
    constexpr bool IndirectlyCopyable = false;
    __stl2::IndirectlyCopyable{In, Out}
    constexpr bool IndirectlyCopyable<In, Out> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // iter_swap [Extension]
  // From the proxy iterator work.
  //
  namespace __iter_swap {
    template <class R1, class R2,
      Readable _R1 = remove_reference_t<R1>,
      Readable _R2 = remove_reference_t<R2>>
        requires Swappable<reference_t<_R1>, reference_t<_R2>>()
    void iter_swap(R1&& r1, R2&& r2)
      noexcept(is_nothrow_swappable_v<reference_t<_R1>, reference_t<_R2>>)
    {
      __stl2::swap(*r1, *r2);
    }

    template <class R1, class R2,
      Readable _R1 = remove_reference_t<R1>,
      Readable _R2 = remove_reference_t<R2>>
        requires IndirectlyMovable<_R1, _R2>() && IndirectlyMovable<_R2, _R1>()
    void iter_swap(R1&& r1, R2&& r2)
      noexcept(is_nothrow_indirectly_movable_v<_R1, _R2> &&
               is_nothrow_indirectly_movable_v<_R2, _R1>) {
      value_type_t<_R1> tmp = __stl2::iter_move(r1);
      *r1 = __stl2::iter_move(r2);
      *r2 = __stl2::move(tmp);
    }

    template <class R1, class R2,
      Readable _R1 = remove_reference_t<R1>,
      Readable _R2 = remove_reference_t<R2>>
        requires IndirectlyMovable<_R1, _R2>() && IndirectlyMovable<_R2, _R1>() &&
          Swappable<reference_t<_R1>, reference_t<_R2>>()
    void iter_swap(R1&& r1, R2&& r2)
      noexcept(is_nothrow_swappable_v<reference_t<_R1>, reference_t<_R2>>)
    {
      __stl2::swap(*r1, *r2);
    }

    template <class R1, class R2>
    constexpr bool can_iter_swap2 = false;
    template <class R1, class R2>
      requires requires (R1&& r1, R1&& r2) {
        iter_swap((R1&&)r1, (R2&&)r2);
      }
    constexpr bool can_iter_swap2<R1, R2> = true;

    struct fn {
      template <class R1, class R2>
        requires can_iter_swap2<R1, R2>
      constexpr void operator()(R1&& r1, R2&& r2) const
      STL2_NOEXCEPT_RETURN(
        (void)iter_swap(__stl2::forward<R1>(r1), __stl2::forward<R2>(r2))
      )
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& iter_swap = detail::static_const<__iter_swap::fn>::value;
  }

  ///////////////////////////////////////////////////////////////////////////
  // IndirectlySwappable [indirectlyswappable.iterators]
  // Not to spec: Alternate formulation from the proxy iterator work.
  //
  template <class I1, class I2>
  constexpr bool __indirectly_swappable = false;
  template <class I1, class I2>
    requires requires (I1 i1, I2 i2) {
      __stl2::iter_swap(i1, i2);
      __stl2::iter_swap(i2, i1);
      __stl2::iter_swap(i1, i1);
      __stl2::iter_swap(i2, i2);
    }
  constexpr bool __indirectly_swappable<I1, I2> = true;

  template <class I1, class I2 = I1>
  concept bool IndirectlySwappable() {
    return Readable<I1>() && Readable<I2>() &&
      __indirectly_swappable<I1, I2>;
  }

  namespace models {
    template <class I1, class I2 = I1>
    constexpr bool IndirectlySwappable = false;
    __stl2::IndirectlySwappable{I1, I2}
    constexpr bool IndirectlySwappable<I1, I2> = true;
  }

  template <class R1, class R2>
  constexpr bool is_nothrow_indirectly_swappable_v = false;

  IndirectlySwappable{R1, R2}
  constexpr bool is_nothrow_indirectly_swappable_v<R1, R2> =
    noexcept(__stl2::iter_swap(declval<R1>(), declval<R2>())) &&
    noexcept(__stl2::iter_swap(declval<R2>(), declval<R1>())) &&
    noexcept(__stl2::iter_swap(declval<R1>(), declval<R1>())) &&
    noexcept(__stl2::iter_swap(declval<R2>(), declval<R2>()));

  template <class R1, class R2>
  using is_nothrow_indirectly_swappable_t =
    meta::bool_<is_nothrow_indirectly_swappable_v<R1, R2>>;

  template <class R1, class R2>
  struct is_nothrow_indirectly_swappable :
    is_nothrow_indirectly_swappable_t<R1, R2> {};

  ///////////////////////////////////////////////////////////////////////////
  // Iterator tags [std.iterator.tags]
  // Extension: contiguous_iterator_tag for denoting contiguous iterators.
  //
  struct output_iterator_tag {};
  struct weak_input_iterator_tag {};
  struct input_iterator_tag :
    weak_input_iterator_tag {};
  struct forward_iterator_tag :
    input_iterator_tag {};
  struct bidirectional_iterator_tag :
    forward_iterator_tag {};
  struct random_access_iterator_tag :
    bidirectional_iterator_tag {};

  namespace ext {
    struct contiguous_iterator_tag :
      random_access_iterator_tag {};
  }

  ///////////////////////////////////////////////////////////////////////////
  // iterator_category and IteratorCategory [iterator.assoc]
  // Extension: Category for pointers is ext::contiguous_iterator_tag,
  //     which derives from random_access_iterator_tag.
  //
  namespace detail {
    template <class T>
    concept bool MemberIteratorCategory =
      requires { typename T::iterator_category; };

    namespace std_to_stl2_iterator_category_ {
      input_iterator_tag f(std::input_iterator_tag*);
      forward_iterator_tag f(std::forward_iterator_tag*);
      bidirectional_iterator_tag f(std::bidirectional_iterator_tag*);
      random_access_iterator_tag f(std::random_access_iterator_tag*);
    }

    template <class T>
    using std_to_stl2_iterator_category =
      decltype(std_to_stl2_iterator_category_::f((T*)nullptr));
  }

  template <class> struct iterator_category {};

  template <_IsNot<is_void> T>
  struct iterator_category<T*> {
    using type = ext::contiguous_iterator_tag;
  };

  template <detail::MemberIteratorCategory T>
  struct iterator_category<T> {
    using type = typename T::iterator_category;
  };

  template <detail::MemberIteratorCategory T>
    requires DerivedFrom<typename T::iterator_category, std::output_iterator_tag>()
  struct iterator_category<T> {};

  template <detail::MemberIteratorCategory T>
    requires DerivedFrom<typename T::iterator_category, std::input_iterator_tag>()
  struct iterator_category<T> {
    using type = detail::std_to_stl2_iterator_category<typename T::iterator_category>;
  };

  template <class T>
  using IteratorCategory =
    meta::_t<iterator_category<remove_cv_t<T>>>;

  ///////////////////////////////////////////////////////////////////////////
  // WeakIterator [weakiterator.iterators]
  //
  template <class I>
  concept bool WeakIterator() {
    return WeaklyIncrementable<I>() &&
      detail::Dereferenceable<I&>;
  }

  namespace models {
    template <class>
    constexpr bool WeakIterator = false;
    __stl2::WeakIterator{I}
    constexpr bool WeakIterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Iterator [iterator.iterators]
  //
  template <class I>
  concept bool Iterator() {
    return WeakIterator<I>() &&
      EqualityComparable<I>();
  }

  namespace models {
    template <class>
    constexpr bool Iterator = false;
    __stl2::Iterator{I}
    constexpr bool Iterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Sentinel [sentinel.iterators]
  //
  template <class S, class I>
  concept bool Sentinel() {
    return Iterator<I>() &&
      Regular<S>() &&
      EqualityComparable<I, S>();
  }

  namespace models {
    template <class, class>
    constexpr bool Sentinel = false;
    __stl2::Sentinel{S, I}
    constexpr bool Sentinel<S, I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // WeakOutputIterator [weakoutput.iterators]
  //
  template <class I, class T>
  concept bool WeakOutputIterator() {
    return WeakIterator<I>() &&
      Writable<I, T>();
  }

  namespace models {
    template <class, class>
    constexpr bool WeakOutputIterator = false;
    __stl2::WeakOutputIterator{I, T}
    constexpr bool WeakOutputIterator<I, T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // OutputIterator [output.iterators]
  //
  template <class I, class T>
  concept bool OutputIterator() {
    return WeakOutputIterator<I, T>() &&
      EqualityComparable<I>();
  }

  namespace models {
    template <class, class>
    constexpr bool OutputIterator = false;
    __stl2::OutputIterator{I, T}
    constexpr bool OutputIterator<I, T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // WeakInputIterator [weakinput.iterators]
  //
  template <class I>
  constexpr bool __weak_input_iterator = false;
  template <class I>
    requires requires (I& i, const I& ci) {
      typename IteratorCategory<I>;
      DerivedFrom<IteratorCategory<I>, weak_input_iterator_tag>();
      STL2_DEDUCTION_CONSTRAINT(i++, Readable);
      requires Same<value_type_t<I>, value_type_t<decltype(i++)>>();
      { *ci } -> const value_type_t<I>&;
    }
  constexpr bool __weak_input_iterator<I> = true;

  template <class I>
  concept bool WeakInputIterator() {
    return WeakIterator<I>() &&
      Readable<I>() &&
      __weak_input_iterator<I>;
  }

  namespace models {
    template <class>
    constexpr bool WeakInputIterator = false;
    __stl2::WeakInputIterator{I}
    constexpr bool WeakInputIterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // InputIterator [input.iterators]
  //
  template <class I>
  concept bool InputIterator() {
    return WeakInputIterator<I>() &&
      Iterator<I>() &&
      DerivedFrom<IteratorCategory<I>, input_iterator_tag>();
  }

  namespace models {
    template <class>
    constexpr bool InputIterator = false;
    __stl2::InputIterator{I}
    constexpr bool InputIterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // ForwardIterator [forward.iterators]
  //
  template <class I>
  concept bool ForwardIterator() {
    return InputIterator<I>() &&
      Incrementable<I>() &&
      DerivedFrom<IteratorCategory<I>, forward_iterator_tag>();
  }

  namespace models {
    template <class>
    constexpr bool ForwardIterator = false;
    __stl2::ForwardIterator{I}
    constexpr bool ForwardIterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // BidirectionalIterator [bidirectional.iterators]
  //
  template <class I>
  concept bool BidirectionalIterator() {
    return ForwardIterator<I>() &&
      DerivedFrom<IteratorCategory<I>, bidirectional_iterator_tag>() &&
      ext::Decrementable<I>();
  }

  namespace models {
    template <class>
    constexpr bool BidirectionalIterator = false;
    __stl2::BidirectionalIterator{I}
    constexpr bool BidirectionalIterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // SizedIteratorRange [sizediteratorrange.iteratorranges]
  //
  template <class I, class S>
  constexpr bool __sized_iterator_range = false;
  template <class I, class S>
    requires requires (const I& i, const S& s) {
      { i - i } -> difference_type_t<I>;
      { s - s } -> difference_type_t<I>;
      { s - i } -> difference_type_t<I>;
      { i - s } -> difference_type_t<I>;
    }
  constexpr bool __sized_iterator_range<I, S> = true;

  template <class I, class S>
  concept bool SizedIteratorRange() {
    return Sentinel<S, I>() &&
      __sized_iterator_range<I, S>;
  }

  namespace models {
    template <class, class>
    constexpr bool SizedIteratorRange = false;
    __stl2::SizedIteratorRange{I, S}
    constexpr bool SizedIteratorRange<I, S> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // RandomAccessIterator [random.access.iterators]
  //
  namespace detail {
    template <class I>
    constexpr bool MutableIterator = false;
    template <class I>
    requires Iterator<I>() &&
      requires (const I& i) {
        STL2_DEDUCE_AUTO_REF(*i);
        *i = *i;
      }
    constexpr bool MutableIterator<I> = true;
  }

  template <class I>
  constexpr bool __random_access_iterator = false;
  template <class I>
    requires requires (const I& ci, const difference_type_t<I> n) {
      STL2_CONVERSION_CONSTRAINT(ci[n], const value_type_t<I>&);
    } &&
      (!detail::MutableIterator<I> ||
       requires (const I& ci, const difference_type_t<I> n) {
         ci[n] = *ci;
         *ci = ci[n];
       })
  constexpr bool __random_access_iterator<I> = true;

  template <class I>
  concept bool RandomAccessIterator() {
    return BidirectionalIterator<I>() &&
      DerivedFrom<IteratorCategory<I>, random_access_iterator_tag>() &&
      TotallyOrdered<I>() &&
      ext::RandomAccessIncrementable<I>() &&
      __random_access_iterator<I>;
  }

  namespace models {
    template <class>
    constexpr bool RandomAccessIterator = false;
    __stl2::RandomAccessIterator{I}
    constexpr bool RandomAccessIterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // ContiguousIterator [Extension]
  //
  namespace ext {
    template <class I>
    concept bool ContiguousIterator() {
      return RandomAccessIterator<I>() &&
        DerivedFrom<IteratorCategory<I>, contiguous_iterator_tag>() &&
        _Is<reference_t<I>, is_reference> &&
        Same<value_type_t<I>, decay_t<reference_t<I>>>();
    }
  }

  namespace models {
    template <class>
    constexpr bool ContiguousIterator = false;
    __stl2::ext::ContiguousIterator{I}
    constexpr bool ContiguousIterator<I> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // iterator_traits [iterator.assoc]
  //
  WeakInputIterator{I}
  struct __pointer_type {
    using type = add_pointer_t<reference_t<I>>;
  };

  template <WeakInputIterator I>
    requires requires (I i) {
      STL2_DEDUCE_AUTO_REF_REF(i.operator->());
    }
  struct __pointer_type<I> {
    using type = decltype(declval<I>().operator->());
  };

  template <class>
  struct __iterator_traits {};

  WeakIterator{I}
  struct __iterator_traits<I> {
    using difference_type = difference_type_t<I>;
    using value_type = void;
    using reference = void;
    using pointer = void;
    using iterator_category = output_iterator_tag;
  };

  WeakInputIterator{I}
  struct __iterator_traits<I> {
    using difference_type = difference_type_t<I>;
    using value_type = value_type_t<I>;
    using reference = reference_t<I>;
    using pointer = meta::_t<__pointer_type<I>>;
    using iterator_category = IteratorCategory<I>;
  };

  template <class I>
  using iterator_traits = __iterator_traits<I>;

  ///////////////////////////////////////////////////////////////////////////
  // Standard iterator traits [iterator.stdtraits]
  // 20150802: Not to spec: Adds constraints to ::iterator_category so as to
  //           apply only to STL2 iterators and avoid "partial specialization
  //           after instantiation" errors.
  //
  template <__stl2::WeakIterator W>
  struct __std_out_value_type {
    using type = void;
  };
  template <__stl2::WeakIterator W>
    requires detail::MemberValueType<W>
  struct __std_out_value_type<W> {
    using type = typename W::value_type;
  };

  template <__stl2::WeakIterator W, class Default = void>
  struct __std_reference_type {
    using type = Default;
  };
  template <__stl2::WeakIterator W, class Default>
    requires requires { typename W::reference; }
  struct __std_reference_type<W, Default> {
    using type = typename W::reference;
  };

  template <__stl2::WeakIterator W, class Default = void>
  struct __std_pointer_type {
    using type = Default;
  };
  template <__stl2::WeakIterator W, class Default>
    requires requires { typename W::pointer; }
  struct __std_pointer_type<W, Default> {
    using type = typename W::pointer;
  };

  namespace detail {
    namespace stl2_to_std_iterator_category_ {
      std::input_iterator_tag f(input_iterator_tag*);
      std::forward_iterator_tag f(forward_iterator_tag*);
      std::bidirectional_iterator_tag f(bidirectional_iterator_tag*);
      std::random_access_iterator_tag f(random_access_iterator_tag*);
    }

    template <class T>
    using stl2_to_std_iterator_category =
      decltype(stl2_to_std_iterator_category_::f((T*)nullptr));
  }
} STL2_CLOSE_NAMESPACE

namespace std {
  template <::__stl2::WeakIterator Out>
    requires !::__stl2::detail::MemberIteratorCategory<Out>
  struct iterator_traits<Out> {
    using difference_type = ::__stl2::difference_type_t<Out>;
    using value_type = meta::_t<::__stl2::__std_out_value_type<Out>>;
    using reference = meta::_t<::__stl2::__std_reference_type<Out>>;
    using pointer = meta::_t<::__stl2::__std_pointer_type<Out>>;
    using iterator_category = std::output_iterator_tag;
  };

  template <::__stl2::WeakInputIterator WI>
    requires ::__stl2::DerivedFrom<typename WI::iterator_category,
                                   ::__stl2::weak_input_iterator_tag>() &&
      !::__stl2::DerivedFrom<typename WI::iterator_category,
                             ::__stl2::input_iterator_tag>()
  struct iterator_traits<WI> {};

  template <::__stl2::InputIterator I>
    requires ::__stl2::DerivedFrom<typename I::iterator_category,
                                   ::__stl2::input_iterator_tag>()
  struct iterator_traits<I> {
    using difference_type = ::__stl2::difference_type_t<I>;
    using value_type = ::__stl2::value_type_t<I>;
    using reference = meta::_t<
      ::__stl2::__std_reference_type<I, ::__stl2::reference_t<I>>>;
    using pointer = meta::_t<
      ::__stl2::__std_pointer_type<I, typename ::__stl2::iterator_traits<I>::pointer>>;
    using iterator_category = meta::if_<
      is_reference<::__stl2::reference_t<I>>,
      ::__stl2::detail::stl2_to_std_iterator_category<typename I::iterator_category>,
      std::input_iterator_tag>;
  };
} // namespace std

#endif
