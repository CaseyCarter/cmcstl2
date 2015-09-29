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
#ifndef STL2_DETAIL_RAW_PTR_HPP
#define STL2_DETAIL_RAW_PTR_HPP

#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
  namespace detail {
    template <class T>
    class raw_ptr {
    public:
#ifdef NDEBUG
      raw_ptr() = default;
#else
      constexpr raw_ptr() noexcept :
        raw_ptr(nullptr) {}
#endif

      constexpr raw_ptr(T* ptr) noexcept :
        ptr_{ptr} {}

      constexpr raw_ptr& operator=(T* ptr) & noexcept {
        ptr_ = ptr;
        return *this;
      }

      constexpr operator T*() const noexcept {
        return ptr_;
      }

      constexpr T& operator*() const noexcept {
        STL2_ASSUME(ptr_);
        return *ptr_;
      }

      constexpr T* operator->() const noexcept {
        STL2_ASSUME(ptr_);
        return ptr_;
      }
    private:
      T* ptr_;
    };
  }
} STL2_CLOSE_NAMESPACE

#endif
