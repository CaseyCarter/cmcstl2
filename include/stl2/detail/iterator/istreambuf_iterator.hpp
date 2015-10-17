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
#ifndef STL2_DETAIL_ITERATOR_ISTREAMBUF_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_ISTREAMBUF_ITERATOR_HPP

#include <iosfwd>
#include <string>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>

STL2_OPEN_NAMESPACE {
  // Not to spec: requirements are implicit.
  template <class charT, class traits = std::char_traits<charT>>
    requires MoveConstructible<charT>() &&
      DefaultConstructible<charT>() &&
      Integral<typename traits::off_type>()
  class istreambuf_iterator {
    class __proxy;
  public:
    using iterator_category = input_iterator_tag;
    using value_type = charT;
    using difference_type = typename traits::off_type;
    using reference = charT;

    using char_type = charT;
    using traits_type = traits;
    using int_type = typename traits::int_type;
    using streambuf_type = std::basic_streambuf<charT, traits>;
    using istream_type = std::basic_istream<charT, traits>;

    class pointer {
    public:
      constexpr charT* operator->() noexcept {
        return &keep_;
      }

    private:
      friend istreambuf_iterator;

      explicit constexpr pointer(const istreambuf_iterator& i)
        noexcept(is_nothrow_move_constructible<charT>::value) :
        keep_{i.get()} {}

      charT keep_;
    };

    constexpr istreambuf_iterator() noexcept :
      sbuf_{nullptr} {}
    STL2_CONSTEXPR_EXT istreambuf_iterator(streambuf_type* s) noexcept :
      sbuf_{s} {}
    STL2_CONSTEXPR_EXT istreambuf_iterator(const __proxy& p) noexcept :
      istreambuf_iterator{p.sbuf_} {}
    istreambuf_iterator(istream_type& s) noexcept :
      istreambuf_iterator{s.rdbuf()} {}

    // Extension
    constexpr istreambuf_iterator(default_sentinel) noexcept :
      istreambuf_iterator() {}

    charT operator*() const {
      return get();
    }
    auto operator->() const {
      return pointer{*this};
    }

    istreambuf_iterator& operator++() & {
      advance();
      return *this;
    }
    auto operator++(int) & {
      return __proxy{traits::to_char_type(advance()), sbuf_};
    }

    STL2_CONSTEXPR_EXT bool equal(const istreambuf_iterator& b) const noexcept {
      return (sbuf_ == nullptr) == (b.sbuf_ == nullptr);
    }

    friend STL2_CONSTEXPR_EXT bool operator==(
      const istreambuf_iterator& lhs, const istreambuf_iterator& rhs) noexcept {
        return lhs.equal(rhs);
    }
    friend STL2_CONSTEXPR_EXT bool operator!=(
      const istreambuf_iterator& lhs, const istreambuf_iterator& rhs) noexcept {
        return !(lhs == rhs);
    }

    // Extensions:
    friend constexpr bool operator==(
      const istreambuf_iterator& lhs, default_sentinel) noexcept {
        return lhs.sbuf_ == nullptr;
    }
    friend constexpr bool operator==(
      default_sentinel lhs, const istreambuf_iterator& rhs) noexcept {
      return rhs == lhs;
    }

    friend constexpr bool operator!=(
      const istreambuf_iterator& lhs, default_sentinel rhs) noexcept {
      return !(lhs == rhs);
    }
    friend constexpr bool operator!=(
      default_sentinel lhs, const istreambuf_iterator& rhs) noexcept {
      return !(rhs == lhs);
    }

  private:
    detail::raw_ptr<streambuf_type> sbuf_;

    charT get() const {
      auto c = sbuf_->sgetc();
      STL2_ASSERT(!traits::eq_int_type(c, traits::eof()));
      return traits::to_char_type(__stl2::move(c));
    }

    int_type advance() {
      auto old_c = sbuf_->sbumpc();
      STL2_ASSERT(!traits::eq_int_type(old_c, traits::eof()));
      if (traits::eq_int_type(sbuf_->sgetc(), traits::eof())) {
        sbuf_ = nullptr;
      }
      return old_c;
    }

    class __proxy {
    public:
      constexpr charT operator*() const
        noexcept(is_nothrow_copy_constructible<charT>::value) {
        return keep_;
      }

    private:
      friend istreambuf_iterator;

      __proxy() = default;
      constexpr __proxy(charT c, streambuf_type* sbuf)
        noexcept(is_nothrow_move_constructible<charT>::value) :
        keep_{__stl2::move(c)}, sbuf_{sbuf} {}

      charT keep_;
      detail::raw_ptr<streambuf_type> sbuf_;
    };
  };
} STL2_CLOSE_NAMESPACE

#endif
