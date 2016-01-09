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
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>

STL2_OPEN_NAMESPACE {
  namespace detail {
    ///////////////////////////////////////////////////////////////////////////
    // ostream_cursor [Implementation detail]
    //
    template <class T, class charT, class traits>
    requires
      models::Same<T, void> ||
      models::StreamInsertable<T, std::basic_ostream<charT, traits>>
    class ostream_cursor {
    public:
      using difference_type = std::ptrdiff_t;
      using ostream_type = std::basic_ostream<charT, traits>;
      struct mixin : protected ebo_box<ostream_cursor> {
        using difference_type = ostream_cursor::difference_type;
        using char_type = charT;
        using traits_type = traits;
        using ostream_type = ostream_cursor::ostream_type;

        mixin() = default;
        using ebo_box<ostream_cursor>::ebo_box;
      };

      constexpr ostream_cursor() noexcept = default;
      STL2_CONSTEXPR_EXT ostream_cursor(
        ostream_type& os, const char* delimiter = nullptr)
      noexcept
      : os_{&os}, delimiter_{delimiter}
      {}

      template <class U = T>
      requires Same<T, U>()
      STL2_CONSTEXPR_EXT void write(const U& u) {
        *os_ << u;
        delimit();
      }

      template <ext::StreamInsertable<ostream_type> U>
      requires Same<T, void>()
      STL2_CONSTEXPR_EXT void write(const U& u) {
        *os_ << u;
        delimit();
      }

    private:
      raw_ptr<ostream_type> os_ = nullptr;
      const char* delimiter_ = nullptr;

      STL2_CONSTEXPR_EXT void delimit() {
        if (delimiter_) {
          *os_ << delimiter_;
        }
      }
    };
  }

  ///////////////////////////////////////////////////////////////////////////
  // ostream_iterator [ostream.iterator]
  // Extension: ostream_iterator<void> accepts any streamable type.
  //
  template <class T = void, class charT = char,
    class traits = std::char_traits<charT>>
  requires
    models::Same<T, void> ||
    models::StreamInsertable<T, std::basic_ostream<charT, traits>>
  using ostream_iterator =
    basic_iterator<detail::ostream_cursor<T, charT, traits>>;
} STL2_CLOSE_NAMESPACE

#endif
