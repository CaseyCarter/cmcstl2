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
#ifndef STL2_DETAIL_FWD_HPP
#define STL2_DETAIL_FWD_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#define STL2_OPEN_NAMESPACE namespace std { namespace experimental { namespace ranges_v1
#define STL2_CLOSE_NAMESPACE }}

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
namespace __stl2 = ::std::experimental::ranges_v1;

STL2_OPEN_NAMESPACE {
  using std::declval;
  using std::forward;

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
    // tags for manually specified overload ordereding
    template <unsigned N>
    struct priority_tag : priority_tag<N - 1> {};
    template <>
    struct priority_tag<0> {};
    // Workaround GCC PR66957 by declaring this unnamed namespace inline.
    inline namespace {
      constexpr auto& max_priority_tag = detail::static_const<priority_tag<2>>::value;
    }
  }
} STL2_CLOSE_NAMESPACE

// Workaround bugs in deduction constraints by replacing:
// * { E } -> T with requires T<decltype(E)>()
// * { E } -> Same<T> with requires Same<decltype(E), T>()
// * { E } -> ConvertibleTo<T> with requires Convertible<decltype(E), T>()
#if 0
#define STL2_DEDUCTION_CONSTRAINT(E, ...) \
  { E } -> __VA_ARGS__

#define STL2_BINARY_DEDUCTION_CONSTRAINT(E, C, ...) \
  STL2_DEDUCTION_CONSTRAINT(E, C<__VA_ARGS__>)

#else
#define STL2_DEDUCTION_CONSTRAINT(E, ...) \
  requires __VA_ARGS__ <decltype(E)>()

#define STL2_BINARY_DEDUCTION_CONSTRAINT(E, C, ...) \
  requires C<decltype(E), __VA_ARGS__>()
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
#if 0
#define STL2_CONSTRAINED_RETURN(...) __VA_ARGS__
#else
#define STL2_CONSTRAINED_RETURN(...) auto
#endif

// Workaround bugs in constrained variable definitions
// (e.g., Iterator x = begin(r);) by simply disabling
// the feature and using "auto"
#if 0
#define STL2_CONSTRAINED_VAR(...) __VA_ARGS__
#else
#define STL2_CONSTRAINED_VAR(...) auto
#endif

#define STL2_NOEXCEPT_RETURN(...) \
  noexcept(noexcept(__VA_ARGS__)) \
  { return __VA_ARGS__; }

#if defined(NDEBUG) && defined(__GNUC__)
// Tell the compiler to optimize on the assumption that the condition holds.
#define STL2_ASSERT(...) (void(!(__VA_ARGS__) && (__builtin_unreachable(), true)))
#elif defined(NDEBUG)
#define STL2_ASSERT(...) (void())
#else
#include <cassert>
#define STL2_ASSERT(...) assert(__VA_ARGS__)
#endif

// Hack asserts that ICE the compiler when
// interpreting constexpr functions.
// (Probably https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66635)
#ifdef NDEBUG
#define STL2_BROKEN_ASSERT(...) STL2_ASSERT(__VA_ARGS__)
#else
#define STL2_BROKEN_ASSERT(...) (void(!(__VA_ARGS__) && (std::terminate(), true)))
#endif

#endif
