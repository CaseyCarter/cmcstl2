// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-2016
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_INCREMENT_HPP
#define STL2_DETAIL_ITERATOR_INCREMENT_HPP

#include <cstddef>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// difference_type_t [iterator.assoc]
	// Not to spec:
	// * Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78173; it is
	//   necessary to guard the requires clause of the "fallback" specialization
	//   to prevent hard errors for pointers to incomplete types.
	//
	namespace detail {
		template <class T>
		concept bool MemberDifferenceType =
			requires { typename T::difference_type; };
	}

	template <class> struct difference_type {};

	template <ext::Object T>
	struct difference_type<T*> {
		using type = std::ptrdiff_t;
	};

	template <class T>
	struct difference_type<const T>
	: difference_type<std::decay_t<T>> {};

	detail::MemberDifferenceType{T}
	struct difference_type<T> {
		using type = typename T::difference_type;
	};

	template <class T>
		requires !detail::MemberDifferenceType<T> &&
			_IsNot<T, is_pointer> && // Avoid GCC PR 78173 (See above)
			requires(const T& a, const T& b) {
				// Avoid gcc ICE (TODO file bug):
				a - b;
				requires Integral<decltype(a - b)>;
			 	// { a - b } -> Integral;
			}
	struct difference_type<T>
	: make_signed<decltype(declval<const T>() - declval<const T>())> {};

	template <class T>
	using difference_type_t = meta::_t<difference_type<T>>;

	///////////////////////////////////////////////////////////////////////////
	// WeaklyIncrementable [weaklyincrementable.iterators]
	//
	template <class I>
	concept bool WeaklyIncrementable =
		Semiregular<I> &&
		requires(I& i) {
			typename difference_type_t<I>;
			{ ++i } -> Same<I&>&&;
			i++;
		};

	namespace models {
		template <class>
		constexpr bool WeaklyIncrementable = false;
		__stl2::WeaklyIncrementable{I}
		constexpr bool WeaklyIncrementable<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Incrementable [incrementable.iterators]
	//
	template <class I>
	concept bool Incrementable =
		WeaklyIncrementable<I> &&
		EqualityComparable<I> &&
		requires(I& i) {
			{ i++ } -> Same<I>&&;
		};

	namespace models {
		template <class>
		constexpr bool Incrementable = false;
		__stl2::Incrementable{I}
		constexpr bool Incrementable<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Decrementable [Extension]
	//
	namespace ext {
		template <class I>
		concept bool Decrementable =
			Incrementable<I> &&
			requires(I& i) {
				{ --i } -> Same<I&>&&;
				{ i-- } -> Same<I>&&;
			};
			// Let a and b be objects of type I.
			// Axiom: &--a == &a
			// Axiom: bool(a == b) implies bool(a-- == b)
			// Axiom: bool(a == b) implies bool((a--, a) == --b)
			// Axiom: bool(a == b) implies bool(--(++a) == b)
			// Axiom: bool(a == b) implies bool(++(--a) == b)
	}

	namespace models {
		template <class>
		constexpr bool Decrementable = false;
		ext::Decrementable{I}
		constexpr bool Decrementable<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// RandomAccessIncrementable [Extension]
	//
	namespace ext {
		template <class I>
		concept bool RandomAccessIncrementable =
			Decrementable<I> &&
			requires(I& i, const I& ci, const difference_type_t<I> n) {
				{ i += n } -> Same<I&>&&;
				{ i -= n } -> Same<I&>&&;
				{ ci + n } -> Same<I>&&;
				{ n + ci } -> Same<I>&&;
				{ ci - n } -> Same<I>&&;
				{ ci - ci } -> difference_type_t<I>;
			};
			// FIXME: Axioms
	}

	namespace models {
		template <class>
		constexpr bool RandomAccessIncrementable = false;
		ext::RandomAccessIncrementable{I}
		constexpr bool RandomAccessIncrementable<I> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
