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
#ifndef STL2_DETAIL_ITERATOR_OSTREAM_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_OSTREAM_ITERATOR_HPP

#include <iosfwd>
#include <string>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
  // Not to spec: StreamInsertable requirement is implicit.
  template <class T, class charT = char,
    class traits = std::char_traits<charT>>
      requires ext::StreamInsertable<T>
  class ostream_iterator {
  public:
    using iterator_category = output_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using char_type = charT;
    using traits_type = traits;
    using ostream_type = std::basic_ostream<charT, traits>;

    constexpr ostream_iterator() noexcept = default;

    STL2_CONSTEXPR_EXT ostream_iterator(ostream_type& s) noexcept :
      stream_{&s} {}

    STL2_CONSTEXPR_EXT ostream_iterator(ostream_type& s, const charT* delimiter) noexcept :
      stream_{&s}, delimiter_{delimiter} {}

    ostream_iterator& operator=(const T& t) & {
      *stream_ << t;
      if (delimiter_) {
        *stream_ << delimiter_;
      }
      return *this;
    }

    STL2_CONSTEXPR_EXT ostream_iterator& operator*() noexcept {
      return *this;
    }

    STL2_CONSTEXPR_EXT ostream_iterator& operator++() & noexcept {
      return *this;
    }

    STL2_CONSTEXPR_EXT ostream_iterator& operator++(int) & noexcept {
      return *this;
    }

  private:
    detail::raw_ptr<ostream_type> stream_ = nullptr;
    const char* delimiter_ = nullptr;
  };
} STL2_CLOSE_NAMESPACE

#endif
