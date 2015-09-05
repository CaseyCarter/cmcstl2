#ifndef STL2_DETAIL_ITERATOR_ISTREAM_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_ISTREAM_ITERATOR_HPP

#include <iosfwd>
#include <string>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>

STL2_OPEN_NAMESPACE {
  // Not to spec: Semiregular and StreamExtractable requirements are implicit.
  template <Semiregular T, class charT = char,
    class traits = std::char_traits<charT>,
    class Distance = std::ptrdiff_t>
      requires ext::StreamExtractable<T>
  class istream_iterator {
  public:
    using iterator_category = input_iterator_tag;
    using difference_type = Distance;
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using char_type = charT;
    using traits_type = traits;
    using istream_type = std::basic_istream<charT, traits>;

    constexpr istream_iterator() noexcept = default;
    istream_iterator(istream_type& s)
      noexcept(is_nothrow_default_constructible<T>::value) :
      stream_{&s} {
      ++*this;
    }

    // Extension
    constexpr istream_iterator(default_sentinel) :
      istream_iterator() {}

    constexpr const T& operator*() const noexcept {
      return value_;
    }
    constexpr const T* operator->() const noexcept {
      return &value_;
    }

    istream_iterator& operator++() & {
      *stream_ >> value_;
      if (!*stream_) {
        stream_ = nullptr;
      }
      return *this;
    }

    istream_iterator operator++(int) & {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    friend constexpr bool operator==(
      const istream_iterator& lhs, const istream_iterator& rhs) noexcept {
        return lhs.stream_ == rhs.stream_;
    }

    friend constexpr bool operator!=(
      const istream_iterator& lhs, const istream_iterator& rhs) noexcept {
        return !(lhs == rhs);
    }

    // Extension
    friend constexpr bool operator==(
      const istream_iterator& lhs, default_sentinel) noexcept {
        return lhs.stream_ == nullptr;
    }
    friend constexpr bool operator==(
      default_sentinel d, const istream_iterator& rhs) noexcept {
        return rhs == d;
    }
    friend constexpr bool operator!=(
      const istream_iterator& lhs, default_sentinel d) noexcept {
        return !(lhs == d);
    }
    friend constexpr bool operator!=(
      default_sentinel d, const istream_iterator& rhs) noexcept {
        return !(rhs == d);
    }

  private:
    detail::raw_ptr<istream_type> stream_ = nullptr;
    T value_{};
  };
} STL2_CLOSE_NAMESPACE

#endif
