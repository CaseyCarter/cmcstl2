// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef RAII_HPP_INCLUDED
#define RAII_HPP_INCLUDED
#include <stl2/memory.hpp>

template <typename T>
class raw_buffer {
public:
   raw_buffer(const std::ptrdiff_t size)
      : data_{allocator_.allocate(size)},
        size_{size}
   {
   }

   raw_buffer(const raw_buffer&) = delete;
   raw_buffer(raw_buffer&&) = default;

   raw_buffer& operator=(const raw_buffer&) = delete;
   raw_buffer& operator=(raw_buffer&&) = default;

   ~raw_buffer()
   {
      allocator_.deallocate(data_, size());
   }

   T* begin() noexcept
   {
      return data_;
   }

   const T* begin() const noexcept
   {
      return data_;
   }

   auto cbegin() const noexcept
   {
      return begin();
   }

   T* end() noexcept
   {
      return data_ + size();
   }

   const T* end() const noexcept
   {
      return data_ + size();
   }

   auto cend() const noexcept
   {
      return end();
   }

   std::ptrdiff_t size() const noexcept
   {
      return size_;
   }
private:
   std::allocator<T> allocator_;
   T* data_;
   std::ptrdiff_t size_;
};

template <typename T>
using Array = std::array<T, 8>;

template <typename T>
raw_buffer<T> make_buffer(const std::size_t size)
{
   return {static_cast<std::ptrdiff_t>(size)};
}
#endif // RAII_HPP_INCLUDED
