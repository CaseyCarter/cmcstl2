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
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	////////////////////////////////////////////////////////////////////////////
	// incrementable_traits [incrementable.traits]
	// Not to spec:
	// * Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78173; it is
	//   necessary to guard the requires clause of the "fallback" specialization
	//   to prevent hard errors for pointers to incomplete types.
	//
	template<class> struct incrementable_traits {};

	template<class T>
	requires std::is_object_v<T>
	struct incrementable_traits<T*> {
		using difference_type = std::ptrdiff_t;
	};

	template<class I>
	struct incrementable_traits<const I> : incrementable_traits<I> {};

	template<class T>
	requires requires { typename T::difference_type; }
	struct incrementable_traits<T> {
		using difference_type = typename T::difference_type;
	};

	template<class T>
	requires (!requires { typename T::difference_type; } &&
		!std::is_pointer_v<T> && // Avoid GCC PR 78173 (See above)
		requires(const T& a, const T& b) {
#ifdef META_HAS_P1084
			{ a - b } -> integral;
#else
			a - b; requires integral<decltype(a - b)>;
#endif
		})
	struct incrementable_traits<T> {
		using difference_type = std::make_signed_t<
			decltype(std::declval<T>() - std::declval<T>())>;
	};

	template<class T>
	using iter_difference_t = typename incrementable_traits<T>::difference_type;

	///////////////////////////////////////////////////////////////////////////
	// weakly_incrementable [weaklyincrementable.iterators]
	//
	template<class I>
	META_CONCEPT weakly_incrementable =
			movable<I> && default_initializable<I> &&
		requires(I i) {
			typename iter_difference_t<I>;
			requires signed_integral<iter_difference_t<I>>;
#ifdef META_HAS_P1084
			{ ++i } -> same_as<I&>; // not required to be equality-preserving
#else
			++i; requires same_as<decltype((++i)), I&>; // not required to be equality-preserving
#endif // META_HAS_P1084
			i++; // not required to be equality-preserving
		};

	///////////////////////////////////////////////////////////////////////////
	// incrementable [incrementable.iterators]
	//
	template<class I>
	META_CONCEPT incrementable =
		regular<I> &&
		weakly_incrementable<I> &&
		requires(I i) {
#ifdef META_HAS_P1084
			{ i++ } -> same_as<I>;
#else
			i++; requires same_as<decltype(i++), I>;
#endif
		};

	///////////////////////////////////////////////////////////////////////////
	// Decrementable [Extension]
	//
	namespace ext {
		template<class I>
		META_CONCEPT Decrementable =
			incrementable<I> &&
			requires(I i) {
#ifdef META_HAS_P1084
				{ --i } -> same_as<I&>;
				{ i-- } -> same_as<I>;
#else
				--i; requires same_as<decltype((--i)), I&>;
				i--; requires same_as<I, decltype(i--)>;
#endif
			};
			// Let a and b be objects of type I.
			// Axiom: &--a == &a
			// Axiom: bool(a == b) implies bool(a-- == b)
			// Axiom: bool(a == b) implies bool((a--, a) == --b)
			// Axiom: bool(a == b) implies bool(--(++a) == b)
			// Axiom: bool(a == b) implies bool(++(--a) == b)
	}

	///////////////////////////////////////////////////////////////////////////
	// RandomAccessIncrementable [Extension]
	//
	namespace ext {
		template<class I>
		META_CONCEPT RandomAccessIncrementable =
			Decrementable<I> &&
			requires(I& i, const I& ci, const iter_difference_t<I> n) {
#ifdef META_HAS_P1084
				{ i += n } -> same_as<I&>;
				{ i -= n } -> same_as<I&>;
				{ ci + n } -> same_as<I>;
				{ n + ci } -> same_as<I>;
				{ ci - n } -> same_as<I>;
#else
				i += n; requires same_as<decltype((i += n)), I&>;
				i -= n; requires same_as<decltype((i -= n)), I&>;
				ci + n; requires same_as<decltype((ci + n)), I>;
				n + ci; requires same_as<decltype((n + ci)), I>;
				ci - n; requires same_as<decltype((ci - n)), I>;
#endif // META_HAS_P1084
				{ ci - ci } -> iter_difference_t<I>;
			};
			// FIXME: Axioms
	}
} STL2_CLOSE_NAMESPACE

#endif
