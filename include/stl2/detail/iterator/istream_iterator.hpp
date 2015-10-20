// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_ISTREAM_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_ISTREAM_ITERATOR_HPP

#include <iosfwd>
#include <string>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>

STL2_OPEN_NAMESPACE {
  // Not to spec: Semiregular, SignedIntegral and StreamExtractable
  // requirements are implicit.
  namespace detail {
    template <Semiregular T, class charT = char,
              class traits = std::char_traits<charT>,
              SignedIntegral Distance = std::ptrdiff_t>
      requires ext::StreamExtractable<T>
    class istream_cursor {
    public:
      using difference_type = Distance;
      using value_type = T;
      using reference = const T&;
      using pointer = const T*;
      using char_type = charT;
      using traits_type = traits;
      using istream_type = std::basic_istream<charT, traits>;

      constexpr istream_cursor() noexcept = default;
      istream_cursor(istream_type& s)
        noexcept(is_nothrow_default_constructible<T>::value) :
        stream_{&s} {
        next();
      }

      // Extension
      constexpr istream_cursor(default_sentinel) :
        istream_cursor() {}

    private:
      detail::raw_ptr<istream_type> stream_ = nullptr;
      T value_{};

      using single_pass = true_type;

      friend cursor_access;
      constexpr reference current() const noexcept {
        return value_;
      }

      void next() {
        *stream_ >> value_;
        if (!*stream_) {
          stream_ = nullptr;
        }
      }

      constexpr bool equal(const istream_cursor& that) const noexcept {
        return stream_ == that.stream_;
      }

      // Extension
      constexpr bool equal(default_sentinel) const noexcept {
        return stream_ == nullptr;
      }
    };
  }

  template <Semiregular T>
  using istream_iterator =
    basic_iterator<detail::istream_cursor<T>>;
} STL2_CLOSE_NAMESPACE

#endif
