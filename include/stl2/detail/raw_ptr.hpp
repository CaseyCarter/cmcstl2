#ifndef STL2_DETAIL_RAW_PTR_HPP
#define STL2_DETAIL_RAW_PTR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>

namespace stl2 {
  inline namespace v1 {
    namespace detail {
      template <class T>
      class raw_ptr {
        T* ptr_;
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
          STL2_ASSERT(ptr_);
          return *ptr_;
        }

        constexpr T* operator->() const noexcept {
          STL2_ASSERT(ptr_);
          return ptr_;
        }
      };
    }
  }
}

#endif