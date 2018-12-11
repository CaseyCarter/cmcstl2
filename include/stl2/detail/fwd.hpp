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

#include <stl2/meta/meta.hpp>

#ifdef __clang__
 #define STL2_HAS_BUILTIN(X) __has_builtin(__builtin_ ## X)
#else // __clang__
 #define STL2_HAS_BUILTIN(X) STL2_HAS_BUILTIN_ ## X
 #if defined(__GNUC__)
  #define STL2_HAS_BUILTIN_unreachable 1
 #endif // __GNUC__
#endif // __clang__

#if !defined(__cpp_concepts) || __cpp_concepts == 0
#error Nothing here will work without support for C++ concepts.
#elif __cpp_concepts <= 201507L
#define STL2_CONCEPT concept bool
#else
#define STL2_CONCEPT concept
#endif

#ifndef STL2_WORKAROUND_GCC_69096
 #if defined(__GNUC__) && __GNUC__ >= 6
  // Return type deduction performed *before* checking constraints.
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69096
  #define STL2_WORKAROUND_GCC_69096 1
 #else
  #define STL2_WORKAROUND_GCC_69096 0
 #endif
#endif

#ifndef STL2_WORKAROUND_GCC_79591
 #if defined(__GNUC__) && __GNUC__ >= 6
  // Overloading function template declarations that differ only in their
  // associated constraints does not work properly when at least one declaration
  // is imported with a using declaration.
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79591
  #define STL2_WORKAROUND_GCC_79591 1
 #else
  #define STL2_WORKAROUND_GCC_79591 0
 #endif
#endif

#ifndef STL2_WORKAROUND_CLANG_37556
 #ifdef __clang__ // Workaround https://bugs.llvm.org/show_bug.cgi?id=37556
  #define STL2_WORKAROUND_CLANG_37556 1
 #else
  #define STL2_WORKAROUND_CLANG_37556 0
 #endif
#endif

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
	inline namespace __cpos {
		// Customization point objects, whose names would otherwise
		// clash with hidden friend functions if they were declared
		// directly in the stl2 namespace.
	}
} STL2_CLOSE_NAMESPACE

// Used to qualify STL2 names
namespace __stl2 = ::std::experimental::ranges;

#define STL2_NOEXCEPT_RETURN(...) \
	noexcept(noexcept(__VA_ARGS__)) \
	{ return __VA_ARGS__; }

#define STL2_REQUIRES_RETURN(...) \
	requires requires { __VA_ARGS__; } \
	{ return (__VA_ARGS__); }

#define STL2_NOEXCEPT_REQUIRES_RETURN(...) \
	noexcept(noexcept(__VA_ARGS__)) \
	requires requires { __VA_ARGS__; } \
	{ return (__VA_ARGS__); }

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

#ifndef STL2_ENSURE
 #ifdef NDEBUG
  #define STL2_ENSURE(...) STL2_ASSUME(__VA_ARGS__)
 #else
  #define STL2_ENSURE(...) STL2_ASSERT(__VA_ARGS__)
 #endif // NDEBUG
#endif // STL2_ENSURE

#define STL2_PRAGMA(X) _Pragma(#X)
#if defined(__GNUC__) || defined(__clang__)
#define STL2_DIAGNOSTIC_PUSH STL2_PRAGMA(GCC diagnostic push)
#define STL2_DIAGNOSTIC_POP STL2_PRAGMA(GCC diagnostic pop)
#define STL2_DIAGNOSTIC_IGNORE(X) STL2_PRAGMA(GCC diagnostic ignored X)
#else
#error unsupported compiler
#endif

STL2_OPEN_NAMESPACE {
	using std::declval;
	using std::forward;

	// Must implement move here instead of using std::move to avoid
	// pulling in the move algorithm.
	template<class T>
	requires true
	constexpr std::remove_reference_t<T>&& move(T&& t) noexcept {
		return static_cast<std::remove_reference_t<T>&&>(t);
	}

	namespace ext {
		// tags for manually specified overload ordering
		template<unsigned N>
		struct priority_tag : priority_tag<N - 1> {};
		template<>
		struct priority_tag<0> {};
		inline constexpr priority_tag<4> max_priority_tag{};
	}

	struct __niebloid {
		explicit __niebloid() = default;
		__niebloid(__niebloid&&) = delete;
		__niebloid& operator=(__niebloid&&) = delete;
	};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_FWD_HPP
