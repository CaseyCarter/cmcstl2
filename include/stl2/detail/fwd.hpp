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
#ifndef STL2_DETAIL_FWD_HPP
#define STL2_DETAIL_FWD_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#ifdef __clang__
#define STL2_HAS_BUILTIN(X) __has_builtin(__builtin_ ## X)

#else // __clang__

#define STL2_HAS_BUILTIN(X) STL2_HAS_BUILTIN_ ## X

#if defined(__GNUC__)
#define STL2_HAS_BUILTIN_unreachable 1
#if __GNUC__ >= 7
#define STL2_HAS_BUILTIN_addressof 1
#endif // __GNUC__ >= 7
#endif // __GNUC__
#endif // __clang__

#define STL2_OPEN_NAMESPACE \
	namespace std { namespace experimental { namespace ranges { inline namespace v1
#define STL2_CLOSE_NAMESPACE }}}

// General namespace structure:
STL2_OPEN_NAMESPACE {
	namespace detail {
		// Implementation details, not to be accessed by user code.
	}
	namespace ext {
		// Supported extensions beyond what is specified in C++ and
		// the Ranges proposal, acceptable for user code to access.
	}
	namespace models {
		// Concept-test predicates. E.g., models::ForwardIterator<I> is true iff
		// I meets the syntactic requirements of ForwardIterator.
	}
} STL2_CLOSE_NAMESPACE

// Used to qualify STL2 names
namespace __stl2 = ::std::experimental::ranges;

// Workaround bugs in deduction constraints by replacing:
// * { E } -> T with requires T<decltype(E)>()
// * { E } -> Same<T> with requires Same<decltype(E), T>()
// * { E } -> ConvertibleTo<T> with requires ConvertibleTo<decltype(E), T>()
#if 0
#define STL2_DEDUCTION_CONSTRAINT(E, ...) \
	{ E } -> __VA_ARGS__

#define STL2_BINARY_DEDUCTION_CONSTRAINT(E, C, ...) \
	STL2_DEDUCTION_CONSTRAINT(E, C<__VA_ARGS__>)

#else
#define STL2_DEDUCTION_CONSTRAINT(E, ...) \
	E; requires __VA_ARGS__ <decltype(E)>()

#define STL2_BINARY_DEDUCTION_CONSTRAINT(E, C, ...) \
	E; requires C<decltype(E), __VA_ARGS__>()
#endif

#define STL2_EXACT_TYPE_CONSTRAINT(E, ...) \
	STL2_BINARY_DEDUCTION_CONSTRAINT(E, Same, __VA_ARGS__)

#define STL2_CONVERSION_CONSTRAINT(E, ...) \
	STL2_BINARY_DEDUCTION_CONSTRAINT(E, ConvertibleTo, __VA_ARGS__)

// Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67384
// Use the expression constraint "deduce_auto_ref_ref(E);" in place
// of the compound constraint "{ E } -> auto&&;"
STL2_OPEN_NAMESPACE {
	namespace detail {
		void deduce_auto_ref(auto&);
		void deduce_auto_ref_ref(auto&&);
	}
} STL2_CLOSE_NAMESPACE

#define STL2_DEDUCE_AUTO_REF(E) \
	::__stl2::detail::deduce_auto_ref(E)

#define STL2_DEDUCE_AUTO_REF_REF(E) \
	::__stl2::detail::deduce_auto_ref_ref(E)

// Workaround bugs in constrained return types
// (e.g., Iterator begin(Range&&);) by simply disabling
// the feature and using "auto"
#if 1
#define STL2_CONSTRAINED_RETURN(...) __VA_ARGS__
#else
#define STL2_CONSTRAINED_RETURN(...) auto
#endif

// Workaround bugs in constrained variable definitions
// (e.g., Iterator x = begin(r);) by simply disabling
// the feature and using "auto"
#if 1
#define STL2_CONSTRAINED_VAR(...) __VA_ARGS__
#else
#define STL2_CONSTRAINED_VAR(...) auto
#endif

#define STL2_NOEXCEPT_RETURN(...) \
	noexcept(noexcept(__VA_ARGS__)) \
	{ return __VA_ARGS__; }

#if STL2_CONSTEXPR_EXTENSIONS
#define STL2_CONSTEXPR_EXT constexpr
#else
#define STL2_CONSTEXPR_EXT inline
#endif

#ifndef STL2_ASSERT
#ifdef NDEBUG
#define STL2_ASSERT(...) void(0)
#else
#include <cassert>
#define STL2_ASSERT(...) assert(__VA_ARGS__)
#endif
#endif

#ifndef STL2_EXPENSIVE_ASSERT
#ifdef STL2_USE_EXPENSIVE_ASSERTS
#define STL2_EXPENSIVE_ASSERT(...) STL2_ASSERT(__VA_ARGS__)
#else
#define STL2_EXPENSIVE_ASSERT(...) void(0)
#endif
#endif

#ifndef STL2_ASSUME
#if STL2_HAS_BUILTIN(assume)
#define STL2_ASSUME(...) __builtin_assume(__VA_ARGS__)
#elif STL2_HAS_BUILTIN(unreachable)
// Tell the compiler to optimize on the assumption that the condition holds.
#define STL2_ASSUME(...) ((__VA_ARGS__) ? void(0) : __builtin_unreachable())
#else
#define STL2_ASSUME(...) void(0)
#endif
#endif

#ifndef STL2_EXPECT
#ifdef NDEBUG
#define STL2_EXPECT(...) STL2_ASSUME(__VA_ARGS__)
#else
#define STL2_EXPECT(...) STL2_ASSERT(__VA_ARGS__)
#endif
#endif

STL2_OPEN_NAMESPACE {
	using std::declval;
	using std::forward;

	// Must implement move here instead of using std::move to avoid
	// pulling in the move algorithm.
	template <class T>
	requires true
	constexpr std::remove_reference_t<T>&& move(T&& t) noexcept {
		return static_cast<std::remove_reference_t<T>&&>(t);
	}

	namespace detail {
		// "constexpr object" ODR workaround from N4381.
		template <class T>
		struct static_const {
			static constexpr T value{};
		};

		template <class T>
		constexpr T static_const<T>::value;
	}

	namespace ext {
		// tags for manually specified overload ordering
		template <unsigned N>
		struct priority_tag : priority_tag<N - 1> {};
		template <>
		struct priority_tag<0> {};
		// Workaround GCC PR66957 by declaring this unnamed namespace inline.
		inline namespace {
			constexpr auto& max_priority_tag = detail::static_const<priority_tag<4>>::value;
		}
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_FWD_HPP
