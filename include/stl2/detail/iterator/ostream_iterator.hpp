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
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>

STL2_OPEN_NAMESPACE {
  namespace detail {
    ///////////////////////////////////////////////////////////////////////////
    // ostream_iterator_base
    // Common base class for ostream_iterator<T> and ostream_iterator<void>
    //
    template <class Derived, class charT, class traits>
    class ostream_iterator_base {
    public:
      using iterator_category = output_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using char_type = charT;
      using traits_type = traits;
      using ostream_type = std::basic_ostream<charT, traits>;

      ostream_iterator_base() noexcept = default;

      STL2_CONSTEXPR_EXT
      ostream_iterator_base(ostream_type& s) noexcept :
        stream_{&s} {}

      STL2_CONSTEXPR_EXT
      ostream_iterator_base(ostream_type& s, const charT* delimiter) noexcept :
        stream_{&s}, delimiter_{delimiter} {}

      STL2_CONSTEXPR_EXT
      Derived& operator*() noexcept {
        return derived();
      }

      STL2_CONSTEXPR_EXT
      Derived& operator++() & noexcept {
        return derived();
      }

      STL2_CONSTEXPR_EXT
      Derived operator++(int) &
        noexcept(is_nothrow_copy_constructible<Derived>::value)
      {
        return derived();
      }

    protected:
      raw_ptr<ostream_type> stream_ = nullptr;
      const char* delimiter_ = nullptr;

      void delimit() {
        if (delimiter_) {
          *stream_ << delimiter_;
        }
      }

    private:
      constexpr Derived& derived() noexcept {
        static_assert(models::DerivedFrom<Derived, ostream_iterator_base>);
        return static_cast<Derived&>(*this);
      }
    };
  }

  ///////////////////////////////////////////////////////////////////////////
  // ostream_iterator [ostream.iterator]
  // Not to spec: StreamInsertable requirement is implicit.
  //
  template <class T = void, class charT = char,
    class traits = std::char_traits<charT>>
    requires Same<T, void>() || ext::StreamInsertable<T>
  class ostream_iterator :
    public detail::ostream_iterator_base<
      ostream_iterator<T, charT, traits>, charT, traits>
  {
    using base_t = typename ostream_iterator::ostream_iterator_base;
    using base_t::delimit;
    using base_t::stream_;
  public:
    constexpr ostream_iterator() = default;
    using base_t::base_t;

    ostream_iterator& operator=(const T& t) & {
      *stream_ << t;
      delimit();
      return *this;
    }
  };

  ///////////////////////////////////////////////////////////////////////////
  // ostream_iterator<void> [Extension]
  //
  template <class charT, class traits>
  class ostream_iterator<void, charT, traits> :
    public detail::ostream_iterator_base<
      ostream_iterator<void, charT, traits>, charT, traits>
  {
    using base_t = typename ostream_iterator::ostream_iterator_base;
    using base_t::delimit;
    using base_t::stream_;
  public:
    constexpr ostream_iterator() = default;
    using base_t::base_t;

    ostream_iterator& operator=(const ext::StreamInsertable& t) & {
      *stream_ << t;
      delimit();
      return *this;
    }
  };
} STL2_CLOSE_NAMESPACE

#endif
