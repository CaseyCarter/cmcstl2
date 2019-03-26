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

////////////////////////////////////////////////////////////////////////////////
// Iterator concepts [iterator.requirements]
//
STL2_OPEN_NAMESPACE {
	template<class T>
	META_CONCEPT __dereferenceable = requires(T& t) {
		// { *t } -> __can_reference;
		*t; typename __with_reference<decltype(*t)>;
	};

	////////////////////////////////////////////////////////////////////////////
	// iter_reference_t [iterator.assoc]
	//
	template<__dereferenceable R>
	using iter_reference_t = decltype(*std::declval<R&>());

	////////////////////////////////////////////////////////////////////////////
	// iter_move
	//
	namespace __iter_move {
		// Not a poison pill, simply a non-ADL block.
		void iter_move(); // undefined

		template<class>
		constexpr bool has_customization = false;
		template<class R>
		requires __dereferenceable<R> &&
			requires(R&& r) {
				// { iter_move(static_cast<R&&>(r)) ->__can_reference;
				iter_move(static_cast<R&&>(r));
				requires __can_reference<decltype(iter_move(static_cast<R&&>(r)))>;
			}
		constexpr bool has_customization<R> = true;

		template<class R>
		using rvalue =
			meta::if_<std::is_reference<R>, std::remove_reference_t<R>&&, std::decay_t<R>>;

		struct fn {
			template<class R>
			requires __dereferenceable<R> && has_customization<R>
			constexpr decltype(auto) operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				iter_move((R&&)r)
			)

			template<class R>
			requires __dereferenceable<R>
			constexpr rvalue<iter_reference_t<R>> operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				static_cast<rvalue<iter_reference_t<R>>>(*r)
			)
		};
	}
	inline namespace __cpos {
		inline constexpr __iter_move::fn iter_move{};
	}

	////////////////////////////////////////////////////////////////////////////
	// iter_rvalue_reference_t [Extension]
	// From the proxy iterator work (P0022).
	//
	template<__dereferenceable R>
	using iter_rvalue_reference_t =
		decltype(iter_move(std::declval<R&>()));

	////////////////////////////////////////////////////////////////////////////
	// readable_traits [readable.traits]
	//
	namespace detail {
		template<class T>
		META_CONCEPT MemberValueType =
			requires { typename T::value_type; };
	}

	template<class T> struct __cond_value_type {};
	template<class T>
	requires std::is_object_v<T>
	struct __cond_value_type<T> {
		using value_type = std::remove_cv_t<T>;
	};

	template<class> struct readable_traits {};

	template<class T>
	struct readable_traits<T*> : __cond_value_type<T> {};

	template<class I>
	requires std::is_array_v<I>
	struct readable_traits<I> {
		using value_type = std::remove_cv_t<std::remove_extent_t<I>>;
	};

	template<class I>
	struct readable_traits<I const> : readable_traits<I> {};

	template<class I>
	requires requires { typename I::value_type; }
	struct readable_traits<I> : __cond_value_type<typename I::value_type> {};

	template<class I>
	requires requires { typename I::element_type; }
	struct readable_traits<I> : __cond_value_type<typename I::element_type> {};

	////////////////////////////////////////////////////////////////////////////
	// iter_value_t [readable.iterators]
	//
	template<class I>
	using iter_value_t = typename readable_traits<I>::value_type;

	////////////////////////////////////////////////////////////////////////////
	// Readable [readable.iterators]
	//
	template<class I>
	META_CONCEPT Readable =
		requires {
			// Associated types
			typename iter_value_t<I>;
			typename iter_reference_t<I>;
			typename iter_rvalue_reference_t<I>;
		} &&
		// Relationships between associated types
		CommonReference<iter_reference_t<I>&&, iter_value_t<I>&> &&
		CommonReference<iter_reference_t<I>&&, iter_rvalue_reference_t<I>&&> &&
		CommonReference<iter_rvalue_reference_t<I>&&, const iter_value_t<I>&>;

	// A generally useful dependent type
	template<Readable I>
	using iter_common_reference_t =
		common_reference_t<iter_reference_t<I>, iter_value_t<I>&>;

	////////////////////////////////////////////////////////////////////////////
	// Writable [iterators.writable]
	//
	template<class Out, class R>
	META_CONCEPT Writable =
		__dereferenceable<Out> &&
		requires(Out&& o, R&& r) {
			*o = static_cast<R&&>(r);
			*static_cast<Out&&>(o) = static_cast<R&&>(r);
			const_cast<const iter_reference_t<Out>&&>(*o) = static_cast<R&&>(r);
			const_cast<const iter_reference_t<Out>&&>(*static_cast<Out&&>(o)) = static_cast<R&&>(r);
		};

	////////////////////////////////////////////////////////////////////////////
	// IndirectlyMovable [alg.req.ind.move]
	//
	template<class In, class Out>
	META_CONCEPT IndirectlyMovable =
		Readable<In> && Writable<Out, iter_rvalue_reference_t<In>>;

	template<class In, class Out>
	constexpr bool is_nothrow_indirectly_movable_v = false;

	template<class Out, IndirectlyMovable<Out> In>
	constexpr bool is_nothrow_indirectly_movable_v<In, Out> =
		noexcept(noexcept(std::declval<iter_reference_t<Out>>()
			= iter_move(std::declval<In>())));

	////////////////////////////////////////////////////////////////////////////
	// IndirectlyMovableStorable [alg.req.ind.move]
	//
	template<class In, class Out>
	META_CONCEPT IndirectlyMovableStorable =
		IndirectlyMovable<In, Out> &&
		Writable<Out, iter_value_t<In>&&> &&
		Movable<iter_value_t<In>> &&
		Constructible<iter_value_t<In>, iter_rvalue_reference_t<In>> &&
		Assignable<iter_value_t<In>&, iter_rvalue_reference_t<In>>;

	template<class In, class Out>
	constexpr bool is_nothrow_indirectly_movable_storable_v = false;

	template<class Out, IndirectlyMovableStorable<Out> In>
	constexpr bool is_nothrow_indirectly_movable_storable_v<In, Out> =
		is_nothrow_indirectly_movable_v<In, Out> &&
		std::is_nothrow_assignable<iter_reference_t<Out>, iter_value_t<In>>::value &&
		std::is_nothrow_constructible<iter_value_t<In>, iter_rvalue_reference_t<In>>::value &&
		std::is_nothrow_assignable<iter_value_t<In>&, iter_rvalue_reference_t<In>>::value;

	////////////////////////////////////////////////////////////////////////////
	// IndirectlyCopyable [alg.req.ind.copy]
	//
	template<class In, class Out>
	META_CONCEPT IndirectlyCopyable =
		Readable<In> && Writable<Out, iter_reference_t<In>>;

	////////////////////////////////////////////////////////////////////////////
	// IndirectlyCopyableStorable [alg.req.ind.copy]
	//
	template<class In, class Out>
	META_CONCEPT IndirectlyCopyableStorable =
		IndirectlyCopyable<In, Out> &&
		Writable<Out, const iter_value_t<In>&> &&
		Copyable<iter_value_t<In>> &&
		Constructible<iter_value_t<In>, iter_reference_t<In>> &&
		Assignable<iter_value_t<In>&, iter_reference_t<In>>;

	////////////////////////////////////////////////////////////////////////////
	// iter_swap
	//
	namespace __iter_swap {
		// Poison pill for iter_swap. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template<class T, class U>
		void iter_swap(T, U) = delete;

		template<class R1, class R2>
		META_CONCEPT has_customization = requires(R1&& r1, R2&& r2) {
			iter_swap(static_cast<R1&&>(r1), static_cast<R2&&>(r2));
		};

		template<class R1, class R2>
		requires SwappableWith<iter_reference_t<R1>, iter_reference_t<R2>>
		constexpr void impl(R1&& r1, R2&& r2)
		STL2_NOEXCEPT_RETURN(
			__stl2::swap(*r1, *r2)
		)

		template<class R1, class R2>
		requires (!SwappableWith<iter_reference_t<R1>, iter_reference_t<R2>> &&
			IndirectlyMovableStorable<R1, R2> &&
			IndirectlyMovableStorable<R2, R1>)
		constexpr void impl(R1& r1, R2& r2)
			noexcept(
				is_nothrow_indirectly_movable_storable_v<R1, R2> &&
				is_nothrow_indirectly_movable_storable_v<R2, R1>)
		{
			iter_value_t<R1> tmp = iter_move(r1);
			*r1 = iter_move(r2);
			*r2 = std::move(tmp);
		}

		struct fn {
			template<class R1, class R2>
			requires has_customization<R1, R2>
			constexpr void operator()(R1&& r1, R2&& r2) const
			STL2_NOEXCEPT_RETURN(
				static_cast<void>(iter_swap((R1&&)r1, (R2&&)r2))
			)

			template<class R1, class R2>
			requires
				Readable<std::remove_reference_t<R1>> &&
				Readable<std::remove_reference_t<R2>> &&
				!has_customization<R1&, R2&> &&
				requires(R1& r1, R2& r2) {
					__iter_swap::impl(r1, r2);
				}
			constexpr void operator()(R1&& r1, R2&& r2) const
			STL2_NOEXCEPT_RETURN(
				__iter_swap::impl(r1, r2)
			)
		};
	}
	inline namespace __cpos {
		inline constexpr __iter_swap::fn iter_swap{};
	}

	////////////////////////////////////////////////////////////////////////////
	// IndirectlySwappable [commonalgoreq.indirectlyswappable]
	//
	template<class I1, class I2 = I1>
	META_CONCEPT IndirectlySwappable =
		requires(I1&& i1, I2&& i2) {
			iter_swap((I1&&)i1, (I2&&)i2);
			iter_swap((I2&&)i2, (I1&&)i1);
			iter_swap((I1&&)i1, (I1&&)i1);
			iter_swap((I2&&)i2, (I2&&)i2);
		};

	////////////////////////////////////////////////////////////////////////////
	// Iterator tags [std.iterator.tags]
	// Extension: contiguous_iterator_tag for denoting contiguous iterators.
	//
	struct output_iterator_tag {};
	struct input_iterator_tag {};
	struct forward_iterator_tag : input_iterator_tag {};
	struct bidirectional_iterator_tag : forward_iterator_tag {};
	struct random_access_iterator_tag : bidirectional_iterator_tag {};
	struct contiguous_iterator_tag : random_access_iterator_tag {};

	////////////////////////////////////////////////////////////////////////////
	// iterator_category and iterator_category_t [iterator.assoc]
	// Extension: Category for pointers is contiguous_iterator_tag,
	//     which derives from random_access_iterator_tag.
	//
	namespace detail {
		template<class T>
		META_CONCEPT MemberIteratorCategory =
			requires { typename T::iterator_category; };

		namespace std_to_stl2_iterator_category_ {
			input_iterator_tag f(std::input_iterator_tag*);
			forward_iterator_tag f(std::forward_iterator_tag*);
			bidirectional_iterator_tag f(std::bidirectional_iterator_tag*);
			random_access_iterator_tag f(std::random_access_iterator_tag*);
		}

		template<class T>
		using std_to_stl2_iterator_category =
			decltype(std_to_stl2_iterator_category_::f((T*)nullptr));
	}

	template<class> struct iterator_category {};

	template<class T>
	struct iterator_category<T*>
	: std::enable_if<std::is_object<T>::value, contiguous_iterator_tag> {};

	template<class T>
	struct iterator_category<const T>
	: iterator_category<T> {};

	template<detail::MemberIteratorCategory T>
	struct iterator_category<T> {
		using type = typename T::iterator_category;
	};

	template<detail::MemberIteratorCategory T>
	requires DerivedFrom<typename T::iterator_category, std::input_iterator_tag>
	struct iterator_category<T> {
		using type = detail::std_to_stl2_iterator_category<typename T::iterator_category>;
	};

	template<detail::MemberIteratorCategory T>
	requires
		DerivedFrom<typename T::iterator_category, std::output_iterator_tag> &&
		!DerivedFrom<typename T::iterator_category, std::input_iterator_tag>
	struct iterator_category<T> {};

	template<class T>
	using iterator_category_t = meta::_t<iterator_category<T>>;

	////////////////////////////////////////////////////////////////////////////
	// Iterator [iterators.iterator]
	//
	// Denotes an element of a range, i.e., is a position.
	//
	template<class I>
	META_CONCEPT Iterator =
		__dereferenceable<I&> && WeaklyIncrementable<I>;
		// Axiom?: i is non-singular iff it denotes an element
		// Axiom?: if i equals j then i and j denote equal elements
		// Axiom?: I{} is in the domain of copy/move construction/assignment
		//        (This should probably be a requirement of the object concepts,
		//         or at least Semiregular.)

	////////////////////////////////////////////////////////////////////////////
	// Sentinel [sentinel.iterators]
	//
	// A relationship between an Iterator and a Semiregular ("sentinel")
	// that denote a range.
	//
	template<class S, class I>
	META_CONCEPT Sentinel =
		Iterator<I> &&
		Semiregular<S> &&
		WeaklyEqualityComparable<S, I>;
		// Axiom: if [i,s) denotes a range then:
		//        * i == s is well-defined
		//        * if bool(i == s) then [i,s) is empty
		//        * if bool(i != s) then:
		//          * i is dereferenceable
		//          * the element denoted by i is the first element of [i,s)
		//          * [++i,s) denotes a range

	////////////////////////////////////////////////////////////////////////////
	// SizedSentinel [iterators.sizedsentinel]
	//
	// Refinement of Sentinel that provides the capability to compute the
	// distance between a Iterator and Sentinel that denote a range in
	// constant time.
	//
	template<class S, class I>
	constexpr bool disable_sized_sentinel = false;

	template<class S, class I>
	META_CONCEPT SizedSentinel =
		Sentinel<S, I> &&
		!disable_sized_sentinel<std::remove_cv_t<S>, std::remove_cv_t<I>> &&
		requires(const I i, const S s) {
			{ s - i } -> Same<iter_difference_t<I>>&&;
			{ i - s } -> Same<iter_difference_t<I>>&&;
			// Axiom: If [i,s) denotes a range and N is the smallest
			//        non-negative integer such that N applications of
			//        ++i make bool(i == s) == true
			//        * if N is representable by iter_difference_t<I> then
			//          s - i is well-defined and equal to N
			//        * if -N is representable by iter_difference_t<I> then
			//          i - s is well-defined and equal to -N
		};

	////////////////////////////////////////////////////////////////////////////
	// OutputIterator [iterators.output]
	//
	template<class I, class T>
	META_CONCEPT OutputIterator =
		Iterator<I> &&
		Writable<I, T> &&
		requires(I& i, T&& t) {
			*i++ = std::forward<T>(t);
		};

	////////////////////////////////////////////////////////////////////////////
	// InputIterator [iterators.input]
	//
	template<class I>
	META_CONCEPT InputIterator =
		Iterator<I> &&
		Readable<I> &&
		requires(I& i, const I& ci) {
			typename iterator_category_t<I>;
			DerivedFrom<iterator_category_t<I>, input_iterator_tag>;
			i++;
		};

	////////////////////////////////////////////////////////////////////////////
	// Exposition-only has-arrow [range.utility.helpers]
	template<class I>
	META_CONCEPT __has_arrow = InputIterator<I> &&
		(std::is_pointer_v<I> || requires(I i) { i.operator->(); });

	////////////////////////////////////////////////////////////////////////////
	// ForwardIterator [iterators.forward]
	//
	template<class I>
	META_CONCEPT ForwardIterator =
		InputIterator<I> &&
		DerivedFrom<iterator_category_t<I>, forward_iterator_tag> &&
		Incrementable<I> &&
		Sentinel<I, I>;
		// Axiom: I{} is equality-preserving and non-singular
		// Axiom: if i equals j then i and j denote the same element.
		// Axiom: if [i,s) denotes a range && bool(i != s) then [i,i+1) denotes
		//        a range and i == j has the same definition space as
		//        i+1 == j
		//        Note: intent is to require == et al to be well-defined over
		//        all iterator values that participate in a range.

	////////////////////////////////////////////////////////////////////////////
	// BidirectionalIterator [iterators.bidirectional]
	//
	template<class I>
	META_CONCEPT BidirectionalIterator =
		ForwardIterator<I> &&
		DerivedFrom<iterator_category_t<I>, bidirectional_iterator_tag> &&
		ext::Decrementable<I>;

	////////////////////////////////////////////////////////////////////////////
	// RandomAccessIterator [iterators.random.access]
	//
	template<class I>
	META_CONCEPT RandomAccessIterator =
		BidirectionalIterator<I> &&
		DerivedFrom<iterator_category_t<I>, random_access_iterator_tag> &&
		SizedSentinel<I, I> &&
		// Should ordering be in SizedSentinel and/or RandomAccessIncrementable?
		StrictTotallyOrdered<I> &&
		ext::RandomAccessIncrementable<I> &&
		requires(const I& ci, const iter_difference_t<I> n) {
			ci[n];
			requires Same<decltype(ci[n]), iter_reference_t<I>>;
		};
		// FIXME: Axioms for definition space of ordering operations. Don't
		// require them to be the same space as ==, since pointers can't meet
		// that requirement. Formulation should be similar to that for == in
		// ForwardIterator, e.g., "if [i,j) denotes a range, i < j et al are
		// well-defined."

	////////////////////////////////////////////////////////////////////////////
	// ContiguousIterator
	//
	template<class I>
	META_CONCEPT ContiguousIterator =
		RandomAccessIterator<I> &&
		DerivedFrom<iterator_category_t<I>, contiguous_iterator_tag> &&
		std::is_lvalue_reference<iter_reference_t<I>>::value &&
		Same<iter_value_t<I>, __uncvref<iter_reference_t<I>>>;

	////////////////////////////////////////////////////////////////////////////
	// iterator_traits [iterator.assoc]
	//
	template<InputIterator I>
	struct __pointer_type {
		using type = std::add_pointer_t<iter_reference_t<I>>;
	};

	template<InputIterator I>
	requires
		requires(I i) {
			// { i.operator->() } -> __can_reference;
			i.operator->(); requires __can_reference<decltype(i.operator->())>;
		}
	struct __pointer_type<I> {
		using type = decltype(std::declval<I&>().operator->());
	};

	template<class>
	struct __iterator_traits {};

	template<Iterator I>
	struct __iterator_traits<I> {
		using difference_type = iter_difference_t<I>;
		using value_type = void;
		using reference = void;
		using pointer = void;
		using iterator_category = output_iterator_tag;
	};

	template<InputIterator I>
	struct __iterator_traits<I> {
		using difference_type = iter_difference_t<I>;
		using value_type = iter_value_t<I>;
		using reference = iter_reference_t<I>;
		using pointer = meta::_t<__pointer_type<I>>;
		using iterator_category = iterator_category_t<I>;
	};

	template<class I>
	using iterator_traits = __iterator_traits<I>;

	////////////////////////////////////////////////////////////////////////////
	// Standard iterator traits [iterator.stdtraits]
	//
	namespace detail {
		namespace stl2_to_std_iterator_category_ {
			template<class Ref, class Cat> Cat g(std::input_iterator_tag*, Cat*);
			template<class Ref, class Cat> Cat g(std::output_iterator_tag*, Cat*);
			template<class Ref, class Cat> std::input_iterator_tag g(void*, Cat*, Ref* = 0);
			template<class Ref> std::input_iterator_tag g(const void*, input_iterator_tag*);
			template<class Ref> std::forward_iterator_tag g(const void*, forward_iterator_tag*);
			template<class Ref> std::bidirectional_iterator_tag g(const void*, bidirectional_iterator_tag*);
			template<class Ref> std::random_access_iterator_tag g(const void*, random_access_iterator_tag*);
		}

		template<class Cat, class Ref>
		using stl2_to_std_iterator_category =
			decltype(stl2_to_std_iterator_category_::g<Ref>((Cat*)0, (Cat*)0));

		template<class T, class U = void>
		struct value_type_with_a_default : meta::id<U> {};
		template<class T, class U>
			requires requires { typename T::value_type; }
		struct value_type_with_a_default<T, U> : meta::id<typename T::value_type> {};

		template<class T, class U = void>
		struct reference_with_a_default : meta::id<U> {};
		template<class T, class U>
			requires requires { typename T::reference; }
		struct reference_with_a_default<T, U> : meta::id<typename T::reference> {};

		template<class T, class U = void>
		struct pointer_with_a_default : meta::id<U> {};
		template<class T, class U>
			requires requires { typename T::pointer; }
		struct pointer_with_a_default<T, U> : meta::id<typename T::pointer> {};

		template<class I>
		META_CONCEPT LooksLikeSTL1Iterator =
			requires {
				typename I::iterator_category;
				requires DerivedFrom<typename I::iterator_category, std::input_iterator_tag> ||
						 DerivedFrom<typename I::iterator_category, std::output_iterator_tag>;
			};
		template<class I>
		META_CONCEPT ProbablySTL2Iterator = !LooksLikeSTL1Iterator<I> && Iterator<I>;
	} // namespace detail
} STL2_CLOSE_NAMESPACE

namespace std {
	template<::__stl2::detail::ProbablySTL2Iterator Out>
		// HACKHACK to avoid partial specialization after instantiation errors. Platform
		// vendors can avoid this hack by fixing up stdlib headers to fwd declare these
		// partial specializations in the same place that std::iterator_traits is first
		// defined.
	struct iterator_traits<Out> {
		using difference_type   = ::__stl2::iter_difference_t<Out>;
		using value_type        = meta::_t<::__stl2::detail::value_type_with_a_default<Out>>;
		using reference         = meta::_t<::__stl2::detail::reference_with_a_default<Out>>;
		using pointer           = meta::_t<::__stl2::detail::pointer_with_a_default<Out>>;
		using iterator_category = std::output_iterator_tag;
	};

	template<::__stl2::detail::ProbablySTL2Iterator In>
	requires ::__stl2::InputIterator<In>
	struct iterator_traits<In> { };

	template<::__stl2::detail::ProbablySTL2Iterator In>
	requires ::__stl2::InputIterator<In> && ::__stl2::Sentinel<In, In>
	struct iterator_traits<In> {
		using difference_type   = ::__stl2::iter_difference_t<In>;
		using value_type        = ::__stl2::iter_value_t<In>;
		using reference         =
			meta::_t<::__stl2::detail::reference_with_a_default<
					In, ::__stl2::iter_reference_t<In>>>;
		using pointer           =
			meta::_t<::__stl2::detail::pointer_with_a_default<In,
					typename ::__stl2::iterator_traits<In>::pointer>>;
		using iterator_category =
			::__stl2::detail::stl2_to_std_iterator_category<
					::__stl2::iterator_category_t<In>,
					::__stl2::iter_reference_t<In>>;
	};
} // namespace std

#ifdef __GLIBCXX__ // HACKHACK: pointer iterator wrappers are contiguous
#include <bits/stl_iterator.h>
STL2_OPEN_NAMESPACE {
	template<class T, class C>
	struct iterator_category<::__gnu_cxx::__normal_iterator<T, C>> {
		using type = __stl2::contiguous_iterator_tag;
	};
} STL2_CLOSE_NAMESPACE
#endif

#endif
